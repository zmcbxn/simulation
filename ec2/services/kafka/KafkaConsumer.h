#pragma once

#include <cppkafka/cppkafka.h>
#include <thread>
#include <atomic>
#include <string>
#include <vector>
#include <memory>
#include <jsoncpp/json/json.h>
#include <functional>

using TopicHandler = std::function<void(const Json::Value&)>;

class KafkaConsumer {
public:
    KafkaConsumer(const std::string& brokers,
                const std::string& groupId, // gruopId 오타 수정
                const std::string& topicName,
                const std::vector<std::string>& fields,
                TopicHandler topicHandler);
    ~KafkaConsumer();

    void start();
    void stop();

private:
    void run();

    std::string topicName_;
    std::vector<std::string> fields_;
    TopicHandler topicHandler_;

    std::unique_ptr<cppkafka::Consumer> consumer_;
    std::thread workerThread_;
    std::atomic<bool> running_;
};