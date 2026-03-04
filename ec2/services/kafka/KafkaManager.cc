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
    // if(!initProducers(brokers)) return false;
    // 나중에 필요한 경우 주석 해제하고 로직 추가
    
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
    producer_ = std::make_unique<KafkaProducer>(brokers);
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

void KafkaManager::dispatch(const std::string& action, const Json::Value& data){
    auto it = topicHandlers_.find(action);
    if(it != topicHandlers_.end()){
        it->second(data);
    }else{
        LOG_WARN << "Unknown action received: " << action;
    }
}

void KafkaManager::registerHandlers(){
    topicHandlers_["processCharacterRequest"] = [this](const Json::Value& data) {
        ServiceFactory::getCharacterService()->processCharacterRequest(
            data["serverId"].asString(),
            data["characterName"].asString(),
            data["type"].asInt(),
            [](const Json::Value& res) {
                LOG_DEBUG << "Character process result: " << res.toStyledString();
            }
        );
    };

    // handler 추가
}