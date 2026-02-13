#include "CharacterService.h"
#include <chrono>
#include <atomic>

void CharacterService::updateRedisUpdateTime(const std::string& sCharacterId){
    auto redisClient = drogon::app().getRedisClient();
    std::string redisKey = "char:" + sCharacterId;
    long long currentTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    redisClient->execCommandAsync(
        [](const drogon::nosql::RedisResult& result) { /* 성공 */ },
        [](const drogon::nosql::RedisException& err) { // RedisError -> RedisException
            LOG_ERROR << "Failed to update Redis: " << err.what();
        },
        "HSET %s update_time %s", // 명령어와 인자는 콜백 뒤에 옵니다.
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

        auto redisClient = drogon::app().getRedisClient();
        for(const auto& row : rows){
            dao_->upsertBaseData(row);

            std::string sCharacterId = row["characterId"].asString();
            std::string redisKey = "char:" + sCharacterId;
            redisClient->execCommandAsync(
                [](const drogon::nosql::RedisResult& result){}, [](const drogon::nosql::RedisException& e){},
                "HSETNX %s update_time 0", redisKey.c_str()
            );
        }


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
        std::string redisKey = "char:" + sCharacterId;

        auto redisClient = drogon::app().getRedisClient();
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
                    Json::Value res;
                    res["status"] = "FRESH";
                    callback(res);
                } else {
                    // 캐시 만료 시 fetchCharacterStatus를 포함한 11개 API 호출 시작
                    this->getFullApiFetch(sCharacterId, sServerId, characterInfo, callback);
                }
            },
            [=, this](const drogon::nosql::RedisException& err) {
                LOG_ERROR << "Redis GET error: " << err.what();
                this->getFullApiFetch(sCharacterId, sServerId, characterInfo, callback);
            },
            "HGET %s update_time", redisKey.c_str()
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
        else { 
            auto jsonPtr = res->getJsonObject();
            if (jsonPtr) { sharedCharacter->statusData = *jsonPtr; checkAndFinalize(); }
            else finalize("Character status JSON is null");
        }
    });

    // 2. 장착 장비
    apiClient_.fetchCharacterEquipment(sServerId, sCharacterId, [=, this](const drogon::HttpResponsePtr& res){
        if (!res || res->getStatusCode() != 200) finalize("Failed to fetch character equipment");
        else { 
            auto jsonPtr = res->getJsonObject();
            if (jsonPtr) { sharedCharacter->equipmentData = *jsonPtr; checkAndFinalize(); }
            else finalize("Character equipment JSON is null");
        }
    });

    // 3. 아바타
    apiClient_.fetchAvatar(sServerId, sCharacterId, [=, this](const drogon::HttpResponsePtr& res){
        if (!res || res->getStatusCode() != 200) finalize("Failed to fetch avatar");
        else { 
            auto jsonPtr = res->getJsonObject();
            if (jsonPtr) { sharedCharacter->avatarData = *jsonPtr; checkAndFinalize(); }
            else finalize("Avatar JSON is null");
        }
    });

    // 4. 크리쳐
    apiClient_.fetchCreature(sServerId, sCharacterId, [=, this](const drogon::HttpResponsePtr& res){
        if (!res || res->getStatusCode() != 200) finalize("Failed to fetch creature");
        else { 
            auto jsonPtr = res->getJsonObject();
            if (jsonPtr) { sharedCharacter->creatureData = *jsonPtr; checkAndFinalize(); }
            else finalize("Creature JSON is null");
        }
    });

    // 5. 휘장
    apiClient_.fetchFlags(sServerId, sCharacterId, [=, this](const drogon::HttpResponsePtr& res){
        if (!res || res->getStatusCode() != 200) finalize("Failed to fetch flags");
        else { 
            auto jsonPtr = res->getJsonObject();
            if (jsonPtr) { sharedCharacter->flagData = *jsonPtr; checkAndFinalize(); }
            else finalize("Flags JSON is null");
        }
    });

    // 6. 안개융화
    apiClient_.fetchMist(sServerId, sCharacterId, [=, this](const drogon::HttpResponsePtr& res){
        if (!res || res->getStatusCode() != 200) finalize("Failed to fetch mist");
        else { 
            auto jsonPtr = res->getJsonObject();
            if (jsonPtr) { sharedCharacter->mistData = *jsonPtr; checkAndFinalize(); }
            else finalize("Mist JSON is null");
        }
    });

    // 7. 스킬/스타일
    apiClient_.fetchSkills(sServerId, sCharacterId, [=, this](const drogon::HttpResponsePtr& res){
        if (!res || res->getStatusCode() != 200) finalize("Failed to fetch skills");
        else { 
            auto jsonPtr = res->getJsonObject();
            if (jsonPtr) { sharedCharacter->skillData = *jsonPtr; checkAndFinalize(); }
            else finalize("Skills JSON is null");
        }
    });

    // 8. 버프 강화 장비
    apiClient_.fetchBuffEquipment(sServerId, sCharacterId, [=, this](const drogon::HttpResponsePtr& res){
        if (!res || res->getStatusCode() != 200) finalize("Failed to fetch buff equipment");
        else { 
            auto jsonPtr = res->getJsonObject();
            if (jsonPtr) { sharedCharacter->buffEquipData = *jsonPtr; checkAndFinalize(); }
            else finalize("Buff equipment JSON is null");
        }
    });

    // 9. 버프 강화 아바타
    apiClient_.fetchBuffAvatar(sServerId, sCharacterId, [=, this](const drogon::HttpResponsePtr& res){
        if (!res || res->getStatusCode() != 200) finalize("Failed to fetch buff avatar");
        else { 
            auto jsonPtr = res->getJsonObject();
            if (jsonPtr) { sharedCharacter->buffAvatarData = *jsonPtr; checkAndFinalize(); }
            else finalize("Buff avatar JSON is null");
        }
    });

    // 10. 버프 강화 크리쳐
    apiClient_.fetchBuffCreature(sServerId, sCharacterId, [=, this](const drogon::HttpResponsePtr& res){
        if (!res || res->getStatusCode() != 200) finalize("Failed to fetch buff creature");
        else { 
            auto jsonPtr = res->getJsonObject();
            if (jsonPtr) { sharedCharacter->buffCreatureData = *jsonPtr; checkAndFinalize(); }
            else finalize("Buff creature JSON is null");
        }
    });

    // 11. 타임라인 (최근 100건)
    apiClient_.fetchTimeline(sServerId, sCharacterId, [=, this](const drogon::HttpResponsePtr& res){
        if (!res || res->getStatusCode() != 200) finalize("Failed to fetch timeline");
        else { 
            auto jsonPtr = res->getJsonObject();
            if (jsonPtr) { sharedCharacter->timelineData = *jsonPtr; checkAndFinalize(); }
            else finalize("Timeline JSON is null");
        }
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
