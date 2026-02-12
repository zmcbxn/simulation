#pragma once

#include <cppkafka/cppkafka.h>
#include <thread>
#include <atomic>
#include <memory>
#include <jsoncpp/json/json.h>
#include "CharacterService.h"

class KafkaConsumer {
public:
    KafkaConsumer(CharacterService& service);
    ~KafkaConsumer();

    void start();
    void stop();

private:
    void run();

    CharacterService& characterService_;
    std::thread workerThread_;
    std::atomic<bool> running_{false};
    
    cppkafka::Configuration config_;
    std::unique_ptr<cppkafka::Consumer> consumer_;
};