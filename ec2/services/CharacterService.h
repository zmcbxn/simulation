#pragma once
#include <drogon/drogon.h>
#include "Character.h"
#include "ApiClient.h"
#include "CharacterDAO.h"
#include "kafka/KafkaProducer.h"
#include <memory>
#include <mutex>
#include <unordered_map>
#include <vector>

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
    void processSearchCharacter(const std::string& characterName, const std::string& userId, const std::string& correlationId, std::function<void(const Json::Value&)> callback);
    void processSearchCharacter(const std::string& characterName, const std::string& userId, std::function<void(const Json::Value&)> callback) {
        processSearchCharacter(characterName, userId, "", std::move(callback));
    }
    void processCharacterRequest(const std::string& serverId, const std::string& characterName, int logicType, const std::string& userId, const std::string& correlationId, std::function<void(const Json::Value&)> callback);
    void getFullApiFetch(const std::string& sCharacterId, const std::string& sServerId, const Json::Value& characterInfo, const std::string& userId, const std::string& correlationId, std::function<void(const Json::Value&)> callback);

private:
    ApiClient apiClient_;
    std::unique_ptr<CharacterDAO> dao_;
    std::shared_ptr<KafkaProducer> kafkaProducer_;

    void saveToDatabase(const Character& character, const Json::Value& characterInfo);
    void publishCharacterSearchReady(const std::string& characterName, const std::vector<std::string>& serverList, const std::string& correlationId);
    void publishCharacterSearchFailed(const std::string& characterName, const std::string& reason, const std::string& correlationId);
    void publishCharacterReady(const std::string& characterId, const std::string& serverId, const std::string& correlationId);
    void publishCharacterFailed(const std::string& characterId, const std::string& serverId, const std::string& reason, const std::string& correlationId);

    // 진행 중인 fetch 추적 — 동일 캐릭터 중복 요청 방지
    // 병목 발생 시 샤딩: Map[0]~Map[N-1] 각각 독립 mutex, characterId 해시 % N으로 분산
    std::mutex                                               inFlightMu_;
    std::unordered_map<std::string, std::vector<std::string>> inFlight_; // key: "serverId:characterId"
};