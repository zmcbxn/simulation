#include "KafkaConsumer.h"
#include <drogon/drogon.h>


KafkaConsumer::KafkaConsumer(CharacterService& service) 
    : characterService_(service) {
    config_ = {
        { "metadata.broker.list", "localhost:9092" },
        { "group.id", "comm-server-group" },
        { "enable.auto.commit", true }
    };
    
    consumer_ = std::make_unique<cppkafka::Consumer>(config_);
}

KafkaConsumer::~KafkaConsumer() {
    stop();
}

void KafkaConsumer::start() {
    if (running_) return;

    running_ = true;
    consumer_->subscribe({"CHARACTER"});

    workerThread_ = std::thread([this]() {this->run();});
    LOG_INFO << "Kakfa Consumer started on topic : CHARACTER";
}

void KafkaConsumer::stop() {
    running_ = false;
    if(workerThread_.joinable()){
        workerThread_.join();
    }
}

void KafkaConsumer::run(){
    while (running_) {
        cppkafka::Message msg = consumer_->poll(std::chrono::milliseconds(1000));

        if (!msg || msg.get_error()) continue;

        std::string payload = msg.get_payload();
        Json::Value root;
        Json::CharReaderBuilder builder;
        std::string errs;
        std::unique_ptr<Json::CharReader> reader(builder.newCharReader());

        if (reader->parse(payload.c_str(), payload.c_str() + payload.size(), &root, &errs)) {
            std::string serverId = root["serverId"].asString();
            std::string characterName = root["characterName"].asString();
            std::string type = root["type"].asString();

            if (type == "REQUEST") {
                LOG_INFO << "Received REQUEST for: " << characterName;

                // characterService_.processSearchCharacter(characterName);
            }
        }
    }
}