#pragma once
#include <drogon/HttpClient.h>
#include <string>
#include <functional>

class ApiClient
{
public:
    ApiClient() = default;
    ~ApiClient() = default;

    void fetchCharacter(const std::string& characterName,
                        const std::string& userId,
                        std::function<void(const drogon::HttpResponsePtr&)> callback);

    void fetchCharacterStatus(const std::string& serverId,
                              const std::string& characterId,
                              const std::string& userId,
                              std::function<void(const drogon::HttpResponsePtr&)> callback);

    void fetchCharacterEquipment(const std::string& serverId,
                                 const std::string& characterId,
                                 const std::string& userId,
                                 std::function<void(const drogon::HttpResponsePtr&)> callback);

    void fetchAvatar(const std::string& serverId,
                     const std::string& characterId,
                     const std::string& userId,
                     std::function<void(const drogon::HttpResponsePtr&)> callback);

    void fetchCreature(const std::string& serverId,
                       const std::string& characterId,
                       const std::string& userId,
                       std::function<void(const drogon::HttpResponsePtr&)> callback);

    void fetchOath(const std::string& serverId,
                   const std::string& characterId,
                   const std::string& userId,
                   std::function<void(const drogon::HttpResponsePtr&)> callback);

    void fetchMist(const std::string& serverId,
                   const std::string& characterId,
                   const std::string& userId,
                   std::function<void(const drogon::HttpResponsePtr&)> callback);

    void fetchSkills(const std::string& serverId,
                     const std::string& characterId,
                     const std::string& userId,
                     std::function<void(const drogon::HttpResponsePtr&)> callback);

    void fetchBuffEquipment(const std::string& serverId,
                            const std::string& characterId,
                            const std::string& userId,
                            std::function<void(const drogon::HttpResponsePtr&)> callback);

    void fetchBuffAvatar(const std::string& serverId,
                         const std::string& characterId,
                         const std::string& userId,
                         std::function<void(const drogon::HttpResponsePtr&)> callback);

    void fetchBuffCreature(const std::string& serverId,
                           const std::string& characterId,
                           const std::string& userId,
                           std::function<void(const drogon::HttpResponsePtr&)> callback);

    void fetchTimeline(const std::string& serverId,
                       const std::string& characterId,
                       const std::string& userId,
                       std::function<void(const drogon::HttpResponsePtr&)> callback,
                       int limit = 10,
                       const std::string& code = "",
                       const std::string& startDate = "",
                       const std::string& endDate = "");

    void fetchTimelineNext(const std::string& serverId,
                           const std::string& characterId,
                           const std::string& userId,
                           const std::string& nextToken,
                           std::function<void(const drogon::HttpResponsePtr&)> callback);

private:
    // basePath는 apikey 파라미터 제외한 경로 (?other_param=... 까지)
    void sendApiRequest(const std::string& userId,
                        drogon::HttpMethod method,
                        const std::string& basePath,
                        const std::function<void(const drogon::HttpResponsePtr&)>& callback);
};
