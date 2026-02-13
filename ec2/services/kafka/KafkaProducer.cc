#include "KafkaProducer.h"
#include <drogon/drogon.h>

KafkaProducer::KafkaProducer(const std::string& brokers){
    cppkafka::Configuration config = { { "metadata.broker.list", brokers } };
    producer_ = std::make_unique<cppkafka::Producer>(config);
}

void KafkaProducer::send(const std::string& topic, const std::string& payload, const std::string& key){
    try{
        cppkafka::MessageBuilder builder(topic);
        if(!key.empty()) builder.key(key);
        builder.payload(payload);

        producer_->produce(builder);
        producer_->flush();
    }catch(const std::exception& e){
        LOG_ERROR << "Producer Send Error: " << e.what();
    }
}