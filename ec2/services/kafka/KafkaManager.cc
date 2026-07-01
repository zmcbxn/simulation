#include "KafkaManager.h"
#include "ServiceFactory.h"
#include <drogon/drogon.h>

KafkaManager::KafkaManager(){
        registerHandlers();
    }

KafkaManager::~KafkaManager(){
    stopAll();
}

void KafkaManager::startAll(){
    for(auto& pair : consumer_){
        pair.second->start();
    }
}

void KafkaManager::stopAll(){
    for(auto& pair : consumer_){
        pair.second->stop();
    }
}

bool KafkaManager::initialize(const std::string& configPath){
    if(!config_.load(configPath)){
        LOG_ERROR << "Fail to load config: " << configPath;
        return false;
    }

    std::string brokers = config_.getValue("Kafka", "brokers");
    std::string groupId = config_.getValue("Kafka", "group_id");

    if(!initConsumers(brokers, groupId)) return false;
    if(!initProducers(brokers)) return false;

    return true;
}

bool KafkaManager::initConsumers(const std::string& brokers, const std::string& groupId){
    std::vector<std::string> cTopics = config_.getVector("Kafka", "consume_topics");

    for (const auto& topicName : cTopics) {
        std::string section = "Topic_" + topicName;
        std::vector<std::string> fields = config_.getVector(section, "fields");
        std::string action = config_.getValue(section, "action");

        auto consumer = std::make_unique<KafkaConsumer>(
            brokers, groupId, topicName, fields, 
            [this, action](const Json::Value& data){
                this->dispatch(action, data);
            }
        );

        consumer_[topicName] = std::move(consumer);
        LOG_INFO << "Initialized Kafka Consumer for topic: " << topicName;
    }
    return true;
}

bool KafkaManager::initProducers(const std::string& brokers){
    producer_ = std::make_shared<KafkaProducer>(brokers);
    std::vector<std::string> pTopics = config_.getVector("Kafka", "produce_topic");
    if(pTopics.empty()){
        LOG_WARN << "No produce topics defined in config.";
        return true;
    }
    for (const auto& topicName : pTopics){
        LOG_INFO << "Initialized Kafka Producer for topic: " << topicName;
    }
    return true;
}

void KafkaManager::dispatch(const std::string& /*action*/, const Json::Value& data){
    std::string msgAction = data.get("action", "").asString();
    auto it = topicHandlers_.find(msgAction);
    if(it != topicHandlers_.end()){
        it->second(data);
    }else{
        LOG_WARN << "Unknown action received: " << msgAction;
    }
}

void KafkaManager::registerHandlers(){
    topicHandlers_["character_search"] = [this](const Json::Value& data) {
        std::string userId        = data.get("userId", "").asString();
        std::string correlationId = data.get("correlationId", "").asString();
        ServiceFactory::getCharacterService()->processSearchCharacter(
            data["characterName"].asString(),
            userId,
            correlationId,
            [](const Json::Value& res) {
                LOG_DEBUG << "character_search result: " << res.toStyledString();
            }
        );
    };

    topicHandlers_["character_detail"] = [this](const Json::Value& data) {
        std::string userId        = data.get("userId", "").asString();
        std::string correlationId = data.get("correlationId", "").asString();
        ServiceFactory::getCharacterService()->processCharacterRequest(
            data["serverId"].asString(),
            data["characterId"].asString(),
            data["type"].asInt(),
            userId,
            correlationId,
            [](const Json::Value& res) {
                LOG_DEBUG << "character_detail result: " << res.toStyledString();
            }
        );
    };

    // TODO: timeline, auction 핸들러 추가
}