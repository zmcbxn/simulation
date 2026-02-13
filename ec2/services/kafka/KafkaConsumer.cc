#include "KafkaConsumer.h"
#include <drogon/drogon.h>

KafkaConsumer::KafkaConsumer(const std::string& brokers,
                            const std::string& groupId,
                            const std::string& topicName,
                            const std::vector<std::string>& fields,
                            TopicHandler topicHandler)
    : topicName_(topicName), 
      fields_(fields),
      topicHandler_(topicHandler), 
      running_(false)
{
    cppkafka::Configuration config = {
        { "metadata.broker.list", brokers },
        { "group.id", groupId },
        { "enable.auto.commit", true }
    };
    
    consumer_ = std::make_unique<cppkafka::Consumer>(config);
}

KafkaConsumer::~KafkaConsumer() {
    stop();
}

void KafkaConsumer::start() {
    if (running_) return;
    running_ = true;
    workerThread_ = std::thread(&KafkaConsumer::run, this);
}

void KafkaConsumer::stop() {
    running_ = false;
    if(workerThread_.joinable()){
        workerThread_.join();
    }
}

void KafkaConsumer::run(){
    consumer_->subscribe({ topicName_ });
    LOG_INFO << "Worker thread started for topic: " << topicName_;

    while (running_) {
        auto msg = consumer_->poll(std::chrono::milliseconds(100));
        if(!msg || msg.get_error()) continue;

        std::string payload = msg.get_payload();
        Json::Value root;
        Json::Reader reader;

        if(reader.parse(payload, root)){
            Json::Value extractedData;
            for(const auto& field : fields_){
                if(root.isMember(field)){
                    extractedData[field] = root[field];
                }
            }

            if(topicHandler_){
                topicHandler_(extractedData);
            }
        }
    }
}