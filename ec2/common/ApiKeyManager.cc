#include "ApiKeyManager.h"
#include <fstream>
#include <stdexcept>

ApiKeyManager::ApiKeyManager() {
    // 1. 여기서 파일을 읽습니다. 실패하면 이 함수 안에서 throw가 발생하므로 
    // 아래 타이머 로직으로 넘어가지 않고 프로그램이 안전하게 멈춥니다.
    loadKeysFromFile("ApiKeys.txt");

    // 2. 1분마다 카운트 초기화 타이머 설정
    drogon::app().getLoop()->runEvery(60.0, [this]() {
        resetCounts();
    });
}

void ApiKeyManager::loadKeysFromFile(const std::string& filename){
    std::ifstream file(filename);
    if(!file.is_open()) {
        // 여기서 이미 에러 처리를 완벽하게 하고 있습니다!
        throw std::runtime_error("Failed to open API keys file: " + filename);
    }

    std::string line;
    while(std::getline(file, line)){
        if(line.empty() || line[0] == '#') continue; 
        addApiKey(line);
    }
    file.close();
}

void ApiKeyManager::addApiKey(const std::string& apiKey) {
    std::lock_guard<std::mutex> lock(mutex_);
    apiKeys_.push_back({apiKey, 0});
}

std::string ApiKeyManager::getApiKey() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if(apiKeys_.empty()) {
        throw std::runtime_error("No API keys available in manager");
    }

    // 가장 적게 사용된 키를 선택 (부하 분산)
    int minIndex = 0;
    for(int i = 1; i < apiKeys_.size(); ++i) {
        if(apiKeys_[i].usageCount < apiKeys_[minIndex].usageCount) {
            minIndex = i;
        }
    }

    apiKeys_[minIndex].usageCount++;
    return apiKeys_[minIndex].apiKey;
}

void ApiKeyManager::resetCounts(){
    std::lock_guard<std::mutex> lock(mutex_);
    for(auto& keyInfo : apiKeys_) {
        keyInfo.usageCount = 0;
    }
}