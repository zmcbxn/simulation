#include "CharacterService.h"
#include <chrono>
#include <atomic>
#include <mutex>
#include <json/json.h>

// ── updateRedisUpdateTime ──────────────────────────────────────────────────────
void CharacterService::updateRedisUpdateTime(const std::string& sCharacterId) {
    auto redisClient = drogon::app().getRedisClient();
    std::string redisKey = "char:" + sCharacterId;
    long long currentTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    redisClient->execCommandAsync(
        [redisClient, redisKey](const drogon::nosql::RedisResult&) {
            redisClient->execCommandAsync(
                [](const drogon::nosql::RedisResult&) {},
                [](const drogon::nosql::RedisException& err) {
                    LOG_ERROR << "Failed to set Redis TTL: " << err.what();
                },
                "EXPIRE %s 604800", redisKey.c_str()
            );
        },
        [](const drogon::nosql::RedisException& err) {
            LOG_ERROR << "Failed to update Redis: " << err.what();
        },
        "HSET %s update_time %s",
        redisKey.c_str(),
        std::to_string(currentTime).c_str()
    );
}

// ── processSearchCharacter ─────────────────────────────────────────────────────
void CharacterService::processSearchCharacter(const std::string& characterName, const std::string& userId, std::function<void(const Json::Value&)> callback)
{
    apiClient_.fetchCharacter(characterName, userId, [=, this](const drogon::HttpResponsePtr& response) {
        if (!response || response->getStatusCode() != 200) {
            Json::Value err;
            err["error"] = "Failed to fetch character data";
            callback(err);
            return;
        }

        const Json::Value& characterJson = *(response->getJsonObject());
        const Json::Value& rows = characterJson["rows"];

        // rows 존재 여부를 먼저 확인한 뒤 DB/Redis 작업 수행
        if (rows.empty()) {
            Json::Value err;
            err["error"] = "Character not found";
            callback(err);
            return;
        }

        for (const auto& row : rows)
            dao_->upsertBaseData(row);

        callback(characterJson);
    });
}

// ── processCharacterRequest ────────────────────────────────────────────────────
void CharacterService::processCharacterRequest(const std::string& serverId, const std::string& characterName, int logicType, const std::string& userId, std::function<void(const Json::Value&)> callback)
{
    apiClient_.fetchCharacter(characterName, userId, [=, this](const drogon::HttpResponsePtr& response) {
        if (!response || response->getStatusCode() != 200) {
            Json::Value err;
            err["error"] = "Failed to fetch character data";
            callback(err);
            return;
        }

        const Json::Value& characterJson = *(response->getJsonObject());
        const Json::Value& rows = characterJson["rows"];

        if (rows.empty()) {
            Json::Value err;
            err["error"] = "Character not found";
            callback(err);
            return;
        }

        Json::Value characterInfo = Json::nullValue;
        for (const auto& row : rows) {
            if (row["serverId"].asString() == serverId) {
                characterInfo = row;
                break;
            }
        }

        if (characterInfo.isNull()) {
            Json::Value err;
            err["error"] = "Character not found in the specified server";
            callback(err);
            return;
        }

        std::string sServerId    = characterInfo["serverId"].asString();
        std::string sCharacterId = characterInfo["characterId"].asString();
        std::string redisKey     = "char:" + sCharacterId;

        auto redisClient = drogon::app().getRedisClient();
        redisClient->execCommandAsync(
            [=, this](const drogon::nosql::RedisResult& result) {
                bool needUpdate = true;
                long long currentTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
                if (!result.isNil()) {
                    long long lastUpdateTime  = std::stoll(result.asString());
                    long long updateInterval  = (logicType == 1) ? 60 : 1800;
                    if (currentTime - lastUpdateTime < updateInterval)
                        needUpdate = false;
                }
                if (!needUpdate) {
                    Json::Value res;
                    res["status"] = "FRESH";
                    callback(res);
                } else {
                    this->getFullApiFetch(sCharacterId, sServerId, characterInfo, userId, callback);
                }
            },
            [=, this](const drogon::nosql::RedisException& err) {
                LOG_ERROR << "Redis GET error: " << err.what();
                this->getFullApiFetch(sCharacterId, sServerId, characterInfo, userId, callback);
            },
            "HGET %s update_time", redisKey.c_str()
        );
    });
}

// ── getFullApiFetch 내부 집계 Context ─────────────────────────────────────────
struct ApiFetchContext {
    std::shared_ptr<Character>              character;
    std::atomic<int>                        remaining{11};
    std::atomic<bool>                       done{false};
    std::mutex                              mu;            // character 필드 동시 쓰기 방지
    trantor::TimerId                        timerId{0};
    std::function<void(const Json::Value&)> callback;
};

// ── fetchWithRetry ─────────────────────────────────────────────────────────────
// abortFlag가 true이면 재시도를 즉시 중단한다.
static void fetchWithRetry(
    int maxRetry,
    std::shared_ptr<std::atomic<bool>> abortFlag,
    std::function<void(std::function<void(const drogon::HttpResponsePtr&)>)> fetchFn,
    std::function<void(const drogon::HttpResponsePtr&)> onDone)
{
    auto attempt  = std::make_shared<int>(0);
    auto doFetch  = std::make_shared<std::function<void()>>();
    *doFetch = [=]() {
        if (abortFlag->load()) return;
        fetchFn([=](const drogon::HttpResponsePtr& res) {
            if (res && res->getStatusCode() == 200) {
                onDone(res);
                return;
            }
            if (abortFlag->load()) return;
            if (++(*attempt) < maxRetry)
                (*doFetch)();
            else
                onDone(nullptr);
        });
    };
    (*doFetch)();
}

// ── getFullApiFetch ────────────────────────────────────────────────────────────
void CharacterService::getFullApiFetch(
    const std::string& sCharacterId,
    const std::string& sServerId,
    const Json::Value& characterInfo,
    const std::string& userId,
    std::function<void(const Json::Value&)> callback)
{
    auto ctx = std::make_shared<ApiFetchContext>();
    ctx->character             = std::make_shared<Character>();
    ctx->character->characterId = sCharacterId;
    ctx->character->serverId    = sServerId;
    ctx->callback               = std::move(callback);

    auto* loop = drogon::app().getLoop();

    // ── 전체 타임아웃 30초 ──────────────────────────────────────────────────
    ctx->timerId = loop->runAfter(30.0, [ctx]() {
        if (ctx->done.exchange(true)) return;
        LOG_WARN << "getFullApiFetch timed out: " << ctx->character->characterId;
        Json::Value err;
        err["error"] = "API fetch timed out";
        ctx->callback(err);
    });

    // ── 에러 확정 (최초 1회만 실행) ────────────────────────────────────────
    auto doAbort = [ctx](const std::string& msg) {
        if (ctx->done.exchange(true)) return;
        drogon::app().getLoop()->invalidateTimer(ctx->timerId);
        LOG_ERROR << "getFullApiFetch aborted: " << msg;
        Json::Value err;
        err["error"] = msg;
        ctx->callback(err);
    };

    // ── 성공 확정 (remaining == 0 일 때 최초 1회만 실행) ───────────────────
    auto doFinalize = [ctx, characterInfo, sCharacterId, sServerId, this]() {
        drogon::app().getLoop()->invalidateTimer(ctx->timerId);
        this->saveToDatabase(*ctx->character, characterInfo);
        this->updateRedisUpdateTime(sCharacterId);
        this->publishCharacterReady(sCharacterId, sServerId);
        ctx->callback(ctx->character->toJson());
    };

    auto checkAndFinalize = [ctx, doFinalize]() {
        if (ctx->done.load()) return;        // abort/timeout이 먼저 완료됨 → 무시
        if (--ctx->remaining == 0) {
            if (!ctx->done.exchange(true))   // 최초 완료자만 doFinalize 호출
                doFinalize();
        }
    };

    // ── 개별 API 콜백 팩토리 ───────────────────────────────────────────────
    auto makeOnDone = [ctx, doAbort, checkAndFinalize](
        const std::string& label,
        std::function<void(const Json::Value&)> store)
    {
        return [ctx, doAbort, checkAndFinalize, label, store](const drogon::HttpResponsePtr& res) {
            if (!res) { doAbort("Failed to fetch " + label + " after retries"); return; }
            auto jsonPtr = res->getJsonObject();
            if (!jsonPtr) { doAbort(label + " response JSON is null"); return; }
            {
                std::lock_guard<std::mutex> lk(ctx->mu);
                store(*jsonPtr);
            }
            checkAndFinalize();
        };
    };

    // ── API 목록 (label / fetchFn / storeFn) ───────────────────────────────
    struct ApiTask {
        std::string label;
        std::function<void(std::function<void(const drogon::HttpResponsePtr&)>)> fetch;
        std::function<void(const Json::Value&)> store;
    };

    std::vector<ApiTask> tasks = {
        {"status",
         [this, sServerId, sCharacterId, userId](auto cb){ apiClient_.fetchCharacterStatus(sServerId, sCharacterId, userId, cb); },
         [ctx](const Json::Value& j){ ctx->character->statusData = j; }},
        {"equipment",
         [this, sServerId, sCharacterId, userId](auto cb){ apiClient_.fetchCharacterEquipment(sServerId, sCharacterId, userId, cb); },
         [ctx](const Json::Value& j){ ctx->character->equipmentData = j; }},
        {"avatar",
         [this, sServerId, sCharacterId, userId](auto cb){ apiClient_.fetchAvatar(sServerId, sCharacterId, userId, cb); },
         [ctx](const Json::Value& j){ ctx->character->avatarData = j; }},
        {"creature",
         [this, sServerId, sCharacterId, userId](auto cb){ apiClient_.fetchCreature(sServerId, sCharacterId, userId, cb); },
         [ctx](const Json::Value& j){ ctx->character->creatureData = j; }},
        {"oath",
         [this, sServerId, sCharacterId, userId](auto cb){ apiClient_.fetchOath(sServerId, sCharacterId, userId, cb); },
         [ctx](const Json::Value& j){ ctx->character->oathData = j; }},
        {"mist",
         [this, sServerId, sCharacterId, userId](auto cb){ apiClient_.fetchMist(sServerId, sCharacterId, userId, cb); },
         [ctx](const Json::Value& j){ ctx->character->mistData = j; }},
        {"skills",
         [this, sServerId, sCharacterId, userId](auto cb){ apiClient_.fetchSkills(sServerId, sCharacterId, userId, cb); },
         [ctx](const Json::Value& j){ ctx->character->skillData = j; }},
        {"buffEquip",
         [this, sServerId, sCharacterId, userId](auto cb){ apiClient_.fetchBuffEquipment(sServerId, sCharacterId, userId, cb); },
         [ctx](const Json::Value& j){ ctx->character->buffEquipData = j; }},
        {"buffAvatar",
         [this, sServerId, sCharacterId, userId](auto cb){ apiClient_.fetchBuffAvatar(sServerId, sCharacterId, userId, cb); },
         [ctx](const Json::Value& j){ ctx->character->buffAvatarData = j; }},
        {"buffCreature",
         [this, sServerId, sCharacterId, userId](auto cb){ apiClient_.fetchBuffCreature(sServerId, sCharacterId, userId, cb); },
         [ctx](const Json::Value& j){ ctx->character->buffCreatureData = j; }},
        {"timeline",
         [this, sServerId, sCharacterId, userId](auto cb){ apiClient_.fetchTimeline(sServerId, sCharacterId, userId, cb); },
         [ctx](const Json::Value& j){ ctx->character->timelineData = j; }},
    };

    // abortFlag aliasing: ctx와 소유권을 공유하면서 ctx->done을 가리키는 shared_ptr
    auto abortFlag = std::shared_ptr<std::atomic<bool>>(ctx, &ctx->done);

    for (auto& task : tasks) {
        fetchWithRetry(3, abortFlag, task.fetch, makeOnDone(task.label, task.store));
    }
}

// ── saveToDatabase ─────────────────────────────────────────────────────────────
void CharacterService::saveToDatabase(const Character& character, const Json::Value& characterInfo)
{
    Json::Value baseDataToSave = characterInfo;
    if (character.statusData.isMember("adventureName"))
        baseDataToSave["adventureName"] = character.statusData["adventureName"];
    if (character.statusData.isMember("guildName"))
        baseDataToSave["guildName"] = character.statusData["guildName"];

    dao_->upsertBaseData(baseDataToSave);
    dao_->upsertStatusData(character.characterId, character.serverId, character.statusData);

    if (character.equipmentData.isMember("equipment") && character.equipmentData["equipment"].isArray())
        dao_->upsertEquipmentData(character.characterId, character.serverId, character.equipmentData["equipment"]);

    if (character.oathData.isMember("oath") && character.oathData["oath"].isObject())
        dao_->upsertOathData(character.characterId, character.serverId, character.oathData);

    if (character.avatarData.isMember("avatar") && character.avatarData["avatar"].isArray())
        dao_->upsertAvatarData(character.characterId, character.serverId, character.avatarData);
    if (character.creatureData.isMember("creature") && character.creatureData["creature"].isObject())
        dao_->upsertCreatureData(character.characterId, character.serverId, character.creatureData);
    if (character.mistData.isMember("mistAssimilation") && character.mistData["mistAssimilation"].isObject())
        dao_->upsertMistData(character.characterId, character.serverId, character.mistData);
    if (character.skillData.isMember("skill") && character.skillData["skill"].isObject())
        dao_->upsertSkillData(character.characterId, character.serverId, character.skillData);

    if (character.buffEquipData.isMember("skill"))
        dao_->upsertBuffEquipData(character.characterId, character.serverId, character.buffEquipData);
    if (character.buffAvatarData.isMember("skill"))
        dao_->upsertBuffAvatarData(character.characterId, character.serverId, character.buffAvatarData);
    if (character.buffCreatureData.isMember("skill"))
        dao_->upsertBuffCreatureData(character.characterId, character.serverId, character.buffCreatureData);
    if (character.timelineData.isMember("timeline"))
        dao_->upsertTimelineData(character.characterId, character.serverId, character.timelineData);
}

// ── publishCharacterReady ──────────────────────────────────────────────────────
void CharacterService::publishCharacterReady(const std::string& characterId, const std::string& serverId)
{
    if (!kafkaProducer_) {
        LOG_WARN << "KafkaProducer not set, skipping publishCharacterReady";
        return;
    }

    Json::Value msg;
    msg["action"]      = "CHARACTER_READY";
    msg["characterId"] = characterId;
    msg["serverId"]    = serverId;

    Json::FastWriter writer;
    kafkaProducer_->send("comm.to.logic", writer.write(msg), characterId);
}
