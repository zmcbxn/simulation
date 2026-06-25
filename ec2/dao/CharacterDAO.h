#pragma once
#include <drogon/orm/DbClient.h>
#include <json/json.h>
#include <memory>

class CharacterDAO {
public:
    explicit CharacterDAO(drogon::orm::DbClientPtr dbClient) : dbClient_(dbClient) {}
    virtual ~CharacterDAO() = default;

    void upsertBaseData(const Json::Value& characterData);
    void upsertStatusData(const std::string& characterId, const std::string& serverId, const Json::Value& statusData);
    void upsertEquipmentData(const std::string& characterId, const std::string& serverId, const Json::Value& equipmentData);
    void upsertOathData(const std::string& characterId, const std::string& serverId, const Json::Value& oathData);
    void upsertAvatarData(const std::string& characterId, const std::string& serverId, const Json::Value& avatarData);
    void upsertCreatureData(const std::string& characterId, const std::string& serverId, const Json::Value& creatureData);
    void upsertMistData(const std::string& characterId, const std::string& serverId, const Json::Value& mistData);
    void upsertSkillData(const std::string& characterId, const std::string& serverId, const Json::Value& skillData);
    void upsertBuffEquipData(const std::string& characterId, const std::string& serverId, const Json::Value& buffEquipData);
    void upsertBuffAvatarData(const std::string& characterId, const std::string& serverId, const Json::Value& buffAvatarData);
    void upsertBuffCreatureData(const std::string& characterId, const std::string& serverId, const Json::Value& buffCreatureData);
    void upsertTimelineData(const std::string& characterId, const std::string& serverId, const Json::Value& timelineData);

private:
    drogon::orm::DbClientPtr dbClient_;
};