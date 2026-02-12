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
                        std::function<void (const drogon::HttpResponsePtr& )> callback);

    void fetchCharacterStatus(const std::string& serverId,
                            const std::string& characterId,
                            std::function<void (const drogon::HttpResponsePtr& )> callback);
    
    void fetchCharacterEquipment(const std::string& serverId,
                                const std::string& characterId,
                                std::function<void (const drogon::HttpResponsePtr& )> callback);
    
    void fetchAvatar(const std::string& serverId,
                    const std::string& characterId,
                    std::function<void (const drogon::HttpResponsePtr& )> callback);

    void fetchCreature(const std::string& serverId,
                        const std::string& characterId,
                        std::function<void (const drogon::HttpResponsePtr& )> callback);
    
    void fetchFlags(const std::string& serverId,
                    const std::string& characterId,
                    std::function<void (const drogon::HttpResponsePtr& )> callback);
    
    void fetchMist(const std::string& serverId,
                    const std::string& characterId,
                    std::function<void (const drogon::HttpResponsePtr& )> callback);

    void fetchSkills(const std::string& serverId,
                        const std::string& characterId,
                        std::function<void (const drogon::HttpResponsePtr& )> callback);
    
    void fetchBuffEquipment(const std::string& serverId,
                            const std::string& characterId,
                            std::function<void (const drogon::HttpResponsePtr& )> callback);

    void fetchBuffAvatar(const std::string& serverId,
                        const std::string& characterId,
                        std::function<void (const drogon::HttpResponsePtr& )> callback);
            
    void fetchBuffCreature(const std::string& serverId,
                            const std::string& characterId,
                            std::function<void (const drogon::HttpResponsePtr& )> callback);

    void fetchTimeline(const std::string& serverId,
                        const std::string& characterId,
                        std::function<void (const drogon::HttpResponsePtr& )> callback,
                        int limit = 10,
                        const std::string& code = "",
                        const std::string& startDate = "",
                        const std::string& endDate = "");

    void fetchTimelineNext(const std::string& serverId,
                            const std::string& characterId,
                            const std::string& nextToken,
                            std::function<void (const drogon::HttpResponsePtr& )> callback);

private:
    void sendApiRequest(drogon::HttpMethod method,
                        const std::string& path,
                        const std::function<void (const drogon::HttpResponsePtr& )>& callback);
};