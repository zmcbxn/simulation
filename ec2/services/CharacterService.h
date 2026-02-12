#pragma once
#include <drogon/drogon.h>
#include "Character.h"
#include "ApiClient.h"
#include "CharacterDAO.h"

class CharacterService {
public:
    CharacterService() {
        dao_ = std::make_unique<CharacterDAO>(drogon::app().getDbClient());
    }
    ~CharacterService() = default; 

    void updateRedisUpdateTime(const std::string& characterId);
    void processSearchCharacter(const std::string& characterName, std::function<void(const Json::Value&)> callback);
    void processCharacterRequest(const std::string& serverId, const std::string& characterName, int logicType, std::function<void(const Json::Value&)> callback);
    void getFullApiFetch(const std::string& sCharacterId, const std::string& sServerId, const Json::Value& characterInfo, std::function<void(const Json::Value&)> callback);
    void loadFromDatabase(const std::string& characterId, const std::string& serverId, std::function<void(const Json::Value&)> callback);

private:
    ApiClient apiClient_;
    std::unique_ptr<CharacterDAO> dao_;
    void saveToDatabase(const Character& character, const Json::Value& characterInfo);
};