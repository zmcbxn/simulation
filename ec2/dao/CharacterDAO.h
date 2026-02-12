#pragma once
#include <drogon/orm/DbClient.h>
#include <jsoncpp/json/json.h>
#include <memory>

class CharacterDAO {
public:
    explicit CharacterDAO(drogon::orm::DbClientPtr dbClient) : dbClient_(dbClient) {}
    virtual ~CharacterDAO() = default;

    void upsertBaseData(const Json::Value& characterData);
    void upsertStatusData(const std::string& characterId, const std::string& serverId, const Json::Value& statusData);
    void upsertEquipmentData(const std::string& characterId, const Json::Value& equipmentData);

private:
    drogon::orm::DbClientPtr dbClient_;
};