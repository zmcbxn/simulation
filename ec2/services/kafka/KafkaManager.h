#pragma once

#include "KafkaConfig.h"
#include "KafkaConsumer.h"
#include "KafkaProducer.h"
#include <map>
#include <memory>
#include <string>
#include <functional>

class KafkaManager{
public:
    KafkaManager();
    ~KafkaManager();

    bool initialize(const std::string& configPath);
    void startAll();
    void stopAll();
    std::shared_ptr<KafkaProducer> getProducer() { return producer_; }

private:
    void registerHandlers();
    void dispatch(const std::string& action, const Json::Value& data);
    bool initConsumers(const std::string& brokers, const std::string& groupId);
    bool initProducers(const std::string& brokers);
    KafkaConfig config_;
    std::map<std::string, std::unique_ptr<KafkaConsumer>> consumer_;
    std::shared_ptr<KafkaProducer> producer_;
    std::map<std::string, TopicHandler> topicHandlers_;
};