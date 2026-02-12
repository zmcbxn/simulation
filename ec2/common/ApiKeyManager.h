#pragma once
#include <string>
#include <vector>
#include <mutex>
#include <drogon/drogon.h>

struct ApiKeyInfo{
    std::string apiKey;
    int usageCount;
};

class ApiKeyManager {
public:
    static ApiKeyManager& instance() {
        static ApiKeyManager instance;
        return instance;
    }

    ApiKeyManager(const ApiKeyManager&) = delete;
    ApiKeyManager& operator=(const ApiKeyManager&) = delete;

    void addApiKey(const std::string& apiKey);
    std::string getApiKey();
    void resetCounts();
    

private:
    ApiKeyManager();
    ~ApiKeyManager() = default;
    void loadKeysFromFile(const std::string& filename);
    std::vector<ApiKeyInfo> apiKeys_;
    std::mutex mutex_;
    const int maxUsagePerKey_ = 1000; // 각 API 키의 최대 사용 횟수
};