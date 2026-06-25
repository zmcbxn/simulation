#pragma once
#include <drogon/drogon.h>
#include "Character.h"
#include "ApiClient.h"
#include "CharacterDAO.h"
#include "kafka/KafkaProducer.h"
#include <memory>
#include <mutex>

class CharacterService {
public:
    CharacterService() {
        dao_ = std::make_unique<CharacterDAO>(drogon::app().getDbClient());
    }
    ~CharacterService() = default;

    void setKafkaProducer(std::shared_ptr<KafkaProducer> producer) {
        kafkaProducer_ = std::move(producer);
    }

    void updateRedisUpdateTime(const std::string& characterId);
    void processSearchCharacter(const std::string& characterName, const std::string& userId, std::function<void(const Json::Value&)> callback);
    void processCharacterRequest(const std::string& serverId, const std::string& characterName, int logicType, const std::string& userId, std::function<void(const Json::Value&)> callback);
    void getFullApiFetch(const std::string& sCharacterId, const std::string& sServerId, const Json::Value& characterInfo, const std::string& userId, std::function<void(const Json::Value&)> callback);

private:
    ApiClient apiClient_;
    std::unique_ptr<CharacterDAO> dao_;
    std::shared_ptr<KafkaProducer> kafkaProducer_;

    void saveToDatabase(const Character& character, const Json::Value& characterInfo);
    void publishCharacterReady(const std::string& characterId, const std::string& serverId);
};