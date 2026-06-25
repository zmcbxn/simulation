#pragma once
#include <string>
#include <vector>
#include <mutex>
#include <functional>
#include <drogon/drogon.h>

struct ApiKeyInfo {
    std::string apiKey;
    int usageCount;
};

class ApiKeyManager {
public:
    static ApiKeyManager& instance() {
        static ApiKeyManager inst;
        return inst;
    }
    ApiKeyManager(const ApiKeyManager&) = delete;
    ApiKeyManager& operator=(const ApiKeyManager&) = delete;

    void addApiKey(const std::string& apiKey);

    // 통합 키 조회: userId 없으면 서버 풀, 있으면 Redis에서 유저 키 조회
    void getApiKey(const std::string& userId,
                   std::function<void(const std::string& apiKey)> callback);

    // 로그인 시 호출 - 유저의 등록 API 키를 Redis에 1시간 캐싱
    void cacheUserApiKey(const std::string& userId,
                         const std::string& apiKey,
                         std::function<void(bool success)> callback);

private:
    ApiKeyManager();
    ~ApiKeyManager() = default;
    void loadKeysFromFile(const std::string& filename);
    std::string pickServerKey(); // 서버 풀에서 최소 사용 키 선택, maxUsage 체크

    std::vector<ApiKeyInfo> serverKeys_;
    std::mutex mutex_;
    const int maxUsagePerKey_ = 1000;
};
