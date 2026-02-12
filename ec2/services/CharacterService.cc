#include "CharacterService.h"
#include <chrono>
#include <atomic>

void CharacterService::updateRedisUpdateTime(const std::string& sCharacterId){
    auto redisClient = drogon::app().getRedisClient();
    std::string redisKey = "character_update_time:" + sCharacterId;
    long long currentTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    redisClient->execCommandAsync(
        [](const drogon::nosql::RedisResult& result) { /* 성공 */ },
        [](const drogon::nosql::RedisException& err) { // RedisError -> RedisException
            LOG_ERROR << "Failed to update Redis: " << err.what();
        },
        "SET %s %s", // 명령어와 인자는 콜백 뒤에 옵니다.
        redisKey.c_str(), 
        std::to_string(currentTime).c_str()
    );
}

void CharacterService::processSearchCharacter(const std::string& characterName, std::function<void(const Json::Value&)> callback)
{
    apiClient_.fetchCharacter(characterName, [=, this](const drogon::HttpResponsePtr& response){
        if (!response || response->getStatusCode() != 200) {
            Json::Value errorPayload;
            errorPayload["error"] = "Failed to fetch character data";
            callback(errorPayload);
            return;
        }

        const Json::Value& characterJson = *(response->getJsonObject());
        const Json::Value& rows = characterJson["rows"];

        if(rows.empty()){
            Json::Value errorPayload;
            errorPayload["error"] = "Character not found";
            callback(errorPayload);
            return;
        }

        // 검색된 전체 캐릭터 리스트(rows)를 리액트로 반환 [cite: 2026-02-10]
        callback(characterJson);
    });
}


void CharacterService::processCharacterRequest(const std::string& serverId, const std::string& characterName, int logicType, std::function<void(const Json::Value&)> callback)
{
    apiClient_.fetchCharacter(characterName, [=, this](const drogon::HttpResponsePtr& response){
        if (!response || response->getStatusCode() != 200) {
            Json::Value errorPayload;
            errorPayload["error"] = "Failed to fetch character data";
            callback(errorPayload);
            return;
        }

        const Json::Value& characterJson = *(response->getJsonObject());
        const Json::Value& rows = characterJson["rows"];

        if(rows.empty()){
            Json::Value errorPayload;
            errorPayload["error"] = "Character not found";
            callback(errorPayload);
            return;
        }

        Json::Value characterInfo = Json::nullValue;
        for (const auto& row : rows) {
            if (row["serverId"].asString() == serverId) {
                characterInfo = row;
                break;
            }
        }

        // 해당 서버에 캐릭터가 없는 경우의 예외 처리
        if (characterInfo.isNull()) {
            Json::Value errorPayload;
            errorPayload["error"] = "Character not found in the specified server";
            callback(errorPayload);
            return;
        }

        std::string sServerId = characterInfo["serverId"].asString();
        std::string sCharacterId = characterInfo["characterId"].asString();

        auto redisClient = drogon::app().getRedisClient();
        std::string redisKey = "character_update_time:" + sCharacterId;

        redisClient->execCommandAsync(
            [=, this](const drogon::nosql::RedisResult& result) {
                bool needUpdate = true;
                long long currentTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
                if (!result.isNil()){
                    long long lastUpdateTime = std::stoll(result.asString());
                    long long updateInterval = (logicType == 1) ? 60 : 300;
                    if (currentTime - lastUpdateTime < updateInterval) {
                        needUpdate = false;
                    }
                } 
                if (!needUpdate){
                    // 캐시 유효 시 DB 로드
                    this->loadFromDatabase(sCharacterId, sServerId, callback);
                } else {
                    // 캐시 만료 시 fetchCharacterStatus를 포함한 11개 API 호출 시작
                    this->getFullApiFetch(sCharacterId, sServerId, characterInfo, callback);
                }
            },
            [=, this](const drogon::nosql::RedisException& err) {
                LOG_ERROR << "Redis GET error: " << err.what();
                this->getFullApiFetch(sCharacterId, sServerId, characterInfo, callback);
            },
            "GET %s", redisKey.c_str()
        );
    });
}






void CharacterService::getFullApiFetch(const std::string& sCharacterId, const std::string& sServerId, const Json::Value& characterInfo, std::function<void(const Json::Value&)> callback)
{
    auto sharedCharacter = std::make_shared<Character>();
    sharedCharacter->serverId = sServerId;
    sharedCharacter->characterId = sCharacterId;

    auto remainingRequests = std::make_shared<std::atomic<int>>(11); // 남은 요청 수 (api call 숫자만큼)
    auto isCalled = std::make_shared<std::atomic<bool>>(false); // 중복 호출 방지

    auto finalize = [=, this](const std::string& errorMsg = "") {
            if (isCalled->exchange(true)) return;
            if (!errorMsg.empty()) {
                Json::Value errorPayload;
                errorPayload["error"] = errorMsg;
                callback(errorPayload);
                return;
            } else {
                this->saveToDatabase(*sharedCharacter, characterInfo);
                updateRedisUpdateTime(sCharacterId);
                callback(sharedCharacter->toJson());
            }
        };
    
    auto checkAndFinalize = [=, this]() {
        if (--(*remainingRequests) == 0) {
            finalize();
        }
    };

    // 1. 능력치
    apiClient_.fetchCharacterStatus(sServerId, sCharacterId, [=, this](const drogon::HttpResponsePtr& res){
        if (!res || res->getStatusCode() != 200) finalize("Failed to fetch character status");
        else { sharedCharacter->statusData = *(res->getJsonObject()); checkAndFinalize(); }
    });

    // 2. 장착 장비
    apiClient_.fetchCharacterEquipment(sServerId, sCharacterId, [=, this](const drogon::HttpResponsePtr& res){
        if (!res || res->getStatusCode() != 200) finalize("Failed to fetch character equipment");
        else { sharedCharacter->equipmentData = *(res->getJsonObject()); checkAndFinalize(); }
    });

    // 3. 아바타
    apiClient_.fetchAvatar(sServerId, sCharacterId, [=, this](const drogon::HttpResponsePtr& res){
        if (!res || res->getStatusCode() != 200) finalize("Failed to fetch avatar");
        else { sharedCharacter->avatarData = *(res->getJsonObject()); checkAndFinalize(); }
    });

    // 4. 크리쳐
    apiClient_.fetchCreature(sServerId, sCharacterId, [=, this](const drogon::HttpResponsePtr& res){
        if (!res || res->getStatusCode() != 200) finalize("Failed to fetch creature");
        else { sharedCharacter->creatureData = *(res->getJsonObject()); checkAndFinalize(); }
    });

    // 5. 휘장
    apiClient_.fetchFlags(sServerId, sCharacterId, [=, this](const drogon::HttpResponsePtr& res){
        if (!res || res->getStatusCode() != 200) finalize("Failed to fetch flags");
        else { sharedCharacter->flagData = *(res->getJsonObject()); checkAndFinalize(); }
    });

    // 6. 안개융화
    apiClient_.fetchMist(sServerId, sCharacterId, [=, this](const drogon::HttpResponsePtr& res){
        if (!res || res->getStatusCode() != 200) finalize("Failed to fetch mist");
        else { sharedCharacter->mistData = *(res->getJsonObject()); checkAndFinalize(); }
    });

    // 7. 스킬/스타일
    apiClient_.fetchSkills(sServerId, sCharacterId, [=, this](const drogon::HttpResponsePtr& res){
        if (!res || res->getStatusCode() != 200) finalize("Failed to fetch skills");
        else { sharedCharacter->skillData = *(res->getJsonObject()); checkAndFinalize(); }
    });

    // 8. 버프 강화 장비
    apiClient_.fetchBuffEquipment(sServerId, sCharacterId, [=, this](const drogon::HttpResponsePtr& res){
        if (!res || res->getStatusCode() != 200) finalize("Failed to fetch buff equipment");
        else { sharedCharacter->buffEquipData = *(res->getJsonObject()); checkAndFinalize(); }
    });

    // 9. 버프 강화 아바타
    apiClient_.fetchBuffAvatar(sServerId, sCharacterId, [=, this](const drogon::HttpResponsePtr& res){
        if (!res || res->getStatusCode() != 200) finalize("Failed to fetch buff avatar");
        else { sharedCharacter->buffAvatarData = *(res->getJsonObject()); checkAndFinalize(); }
    });

    // 10. 버프 강화 크리쳐
    apiClient_.fetchBuffCreature(sServerId, sCharacterId, [=, this](const drogon::HttpResponsePtr& res){
        if (!res || res->getStatusCode() != 200) finalize("Failed to fetch buff creature");
        else { sharedCharacter->buffCreatureData = *(res->getJsonObject()); checkAndFinalize(); }
    });

    // 11. 타임라인 (최근 100건)
    apiClient_.fetchTimeline(sServerId, sCharacterId, [=, this](const drogon::HttpResponsePtr& res){
        if (!res || res->getStatusCode() != 200) finalize("Failed to fetch timeline");
        else { sharedCharacter->timelineData = *(res->getJsonObject()); checkAndFinalize(); }
    });
}

void CharacterService::saveToDatabase(const Character& character, const Json::Value& characterInfo)
{
    Json::Value baseDataToSave = characterInfo;

    if(character.statusData.isMember("adventureName")){
        baseDataToSave["adventureName"] = character.statusData["adventureName"];
    }
    if(character.statusData.isMember("guildName")){
        baseDataToSave["guildName"] = character.statusData["guildName"];
    }
    // 기본 정보 저장
    dao_->upsertBaseData(baseDataToSave);
    dao_->upsertStatusData(character.characterId, character.serverId, character.statusData);
    if (character.equipmentData.isMember("equipment") && character.equipmentData["equipment"].isArray()){
        dao_->upsertEquipmentData(character.characterId, character.equipmentData["equipment"]);
    }
    // 추가 데이터는 필요에 따라 별도의 DAO 메서드를 구현하여 저장할 수 있습니다.

}

void CharacterService::loadFromDatabase(const std::string& characterId, const std::string& serverId, std::function<void(const Json::Value&)> callback)
{
    Json::Value res;
    res["info"] = "see you later";
    callback(res);
}