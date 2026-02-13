#include <drogon/drogon.h>
#include "KafkaManager.h"
#include "CharacterService.h"

int main() {
    
    drogon::app().setLogLevel(trantor::Logger::kDebug);
    LOG_DEBUG << "Starting drogon application...";
    
    //drogon::app().loadConfigFile("../config.json");
    //drogon::app().loadConfigFile("../config.yaml");
    drogon::app().loadConfigFile("../config/settings.json");
    
    drogon::app().run();
    return 0;
}
