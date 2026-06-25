#include "ApiKeyManager.h"
#include <fstream>
#include <stdexcept>

ApiKeyManager::ApiKeyManager() {
    loadKeysFromFile("ApiKeys.txt");
    drogon::app().getLoop()->runEvery(60.0, [this]() {
        std::lock_guard<std::mutex> lock(mutex_);
        for (auto& k : serverKeys_) k.usageCount = 0;
    });
}

void ApiKeyManager::loadKeysFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open())
        throw std::runtime_error("Failed to open API keys file: " + filename);
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        addApiKey(line);
    }
}

void ApiKeyManager::addApiKey(const std::string& apiKey) {
    std::lock_guard<std::mutex> lock(mutex_);
    serverKeys_.push_back({apiKey, 0});
}

std::string ApiKeyManager::pickServerKey() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (serverKeys_.empty())
        throw std::runtime_error("No server API keys available");

    int minIdx = -1;
    for (int i = 0; i < (int)serverKeys_.size(); ++i) {
        if (serverKeys_[i].usageCount >= maxUsagePerKey_) continue;
        if (minIdx == -1 || serverKeys_[i].usageCount < serverKeys_[minIdx].usageCount)
            minIdx = i;
    }
    if (minIdx == -1)
        throw std::runtime_error("All server API keys have reached usage limit (1000/min)");

    serverKeys_[minIdx].usageCount++;
    return serverKeys_[minIdx].apiKey;
}

void ApiKeyManager::getApiKey(const std::string& userId,
                               std::function<void(const std::string&)> callback) {
    if (userId.empty()) {
        try {
            callback(pickServerKey());
        } catch (const std::exception& e) {
            LOG_ERROR << "ApiKeyManager: " << e.what();
            callback("");
        }
        return;
    }

    // 로그인 유저: Redis에서 캐싱된 키 조회
    auto redis = drogon::app().getRedisClient();
    redis->execCommandAsync(
        [callback, userId](const drogon::nosql::RedisResult& r) {
            if (!r.isNil() && !r.asString().empty()) {
                callback(r.asString());
            } else {
                // TODO: Redis 미스 시 DB(users 테이블)에서 조회 후 cacheUserApiKey 호출
                // 현재는 서버 풀로 폴백
                LOG_WARN << "No cached API key for user: " << userId << ", falling back to server pool";
                try {
                    ApiKeyManager::instance().getApiKey("", callback);
                } catch (...) {
                    callback("");
                }
            }
        },
        [callback](const drogon::nosql::RedisException& e) {
            LOG_ERROR << "Redis error in getApiKey: " << e.what();
            callback("");
        },
        "GET user:%s:apikey", userId.c_str()
    );
}

void ApiKeyManager::cacheUserApiKey(const std::string& userId,
                                     const std::string& apiKey,
                                     std::function<void(bool)> callback) {
    auto redis = drogon::app().getRedisClient();
    redis->execCommandAsync(
        [callback](const drogon::nosql::RedisResult&) {
            callback(true);
        },
        [callback](const drogon::nosql::RedisException& e) {
            LOG_ERROR << "Redis error in cacheUserApiKey: " << e.what();
            callback(false);
        },
        "SET user:%s:apikey %s EX 3600", userId.c_str(), apiKey.c_str()
    );
}
