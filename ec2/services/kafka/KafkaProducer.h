#pragma once

#include <cppkafka/cppkafka.h>
#include <string>
#include <memory>

class KafkaProducer{
public:
    KafkaProducer(const std::string& brokers);
    ~KafkaProducer() = default;

    void send(const std::string& topic, const std::string& payload, const std::string& key = "");

private:
    std::unique_ptr<cppkafka::Producer> producer_;
};