#include <drogon/drogon.h>
#include "KafkaManager.h"
#include "services/ServiceFactory.h"

int main() {
    
    drogon::app().setLogLevel(trantor::Logger::kDebug);
    LOG_DEBUG << "Starting drogon application...";
    
    //drogon::app().loadConfigFile("../config.json");
    //drogon::app().loadConfigFile("../config.yaml");
    drogon::app().loadConfigFile("../config/settings.json");
    

    auto kafkaManager = std::make_shared<KafkaManager>();
    if(kafkaManager->initialize("GW.ini")){
        kafkaManager->startAll();
    }

    // DB/Redis 초기화 완료 후 CharacterService 생성 및 KafkaProducer 주입
    drogon::app().registerBeginningAdvice([kafkaManager]() {
        if(kafkaManager->getProducer())
            ServiceFactory::getCharacterService()->setKafkaProducer(kafkaManager->getProducer());
    });

    drogon::app().run();
    return 0;
}
