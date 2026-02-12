#include "ApiClient.h"
#include "ApiKeyManager.h"

void ApiClient::sendApiRequest(drogon::HttpMethod method,
                               const std::string& path,
                               const std::function<void (const drogon::HttpResponsePtr& )>& callback)
{
    auto client = drogon::HttpClient::newHttpClient("https://api.neople.co.kr");

    auto req = drogon::HttpRequest::newHttpRequest();
    req->setMethod(method);
    req->setPath(path);

    client->sendRequest(req, [callback](drogon::ReqResult result, const drogon::HttpResponsePtr& resp) {
        if (result == drogon::ReqResult::Ok) {
            callback(resp);
        } else {
            callback(nullptr); // 실패 시 null 전달
        }
    });
}


void ApiClient::fetchCharacter(const std::string& characterName,
                              std::function<void (const drogon::HttpResponsePtr& )> callback)
{
    std::string apiKey = ApiKeyManager::instance().getApiKey();
    std::string path = "/df/servers/all/characters?characterName=" + characterName + "&apikey=" + apiKey;

    sendApiRequest(drogon::Get, path, callback);
}

void ApiClient::fetchCharacterStatus(const std::string& serverId,
                                    const std::string& characterId,
                                    std::function<void (const drogon::HttpResponsePtr& )> callback)
{
    std::string apiKey = ApiKeyManager::instance().getApiKey();
    std::string path = "/df/servers/" + serverId + "/characters/" + characterId + "/status?apikey=" + apiKey;
    
    sendApiRequest(drogon::Get, path, callback);
}

void ApiClient::fetchCharacterEquipment(const std::string& serverId,
                                        const std::string& characterId,
                                        std::function<void (const drogon::HttpResponsePtr& )> callback)
{
    std::string apiKey = ApiKeyManager::instance().getApiKey();
    std::string path = "/df/servers/" + serverId + "/characters/" + characterId + "/equip/equipment?apikey=" + apiKey;

    sendApiRequest(drogon::Get, path, callback);
}

void ApiClient::fetchAvatar(const std::string& serverId,
                            const std::string& characterId,
                            std::function<void (const drogon::HttpResponsePtr& )> callback)
{
    std::string apiKey = ApiKeyManager::instance().getApiKey();
    std::string path = "/df/servers/" + serverId + "/characters/" + characterId + "/equip/avatar?apikey=" + apiKey;

    sendApiRequest(drogon::Get, path, callback);
}

void ApiClient::fetchCreature(const std::string& serverId,
                              const std::string& characterId,
                              std::function<void (const drogon::HttpResponsePtr& )> callback)
{
    std::string apiKey = ApiKeyManager::instance().getApiKey();
    std::string path = "/df/servers/" + serverId + "/characters/" + characterId + "/equip/creature?apikey=" + apiKey;

    sendApiRequest(drogon::Get, path, callback);
}

void ApiClient::fetchFlags(const std::string& serverId,
                            const std::string& characterId,
                            std::function<void (const drogon::HttpResponsePtr& )> callback)
{
    std::string apiKey = ApiKeyManager::instance().getApiKey();
    std::string path = "/df/servers/" + serverId + "/characters/" + characterId + "/equip/flag?apikey=" + apiKey;

    sendApiRequest(drogon::Get, path, callback);
}

void ApiClient::fetchMist(const std::string& serverId,
                            const std::string& characterId,
                            std::function<void (const drogon::HttpResponsePtr& )> callback)
{
    std::string apiKey = ApiKeyManager::instance().getApiKey();
    std::string path = "/df/servers/" + serverId + "/characters/" + characterId + "/equip/mist-assimilation?apikey=" + apiKey;

    sendApiRequest(drogon::Get, path, callback);
}


void ApiClient::fetchSkills(const std::string& serverId,
                            const std::string& characterId,
                            std::function<void (const drogon::HttpResponsePtr& )> callback)
{
    std::string apiKey = ApiKeyManager::instance().getApiKey();
    std::string path = "/df/servers/" + serverId + "/characters/" + characterId + "/skill/style?apikey=" + apiKey;

    sendApiRequest(drogon::Get, path, callback);
}

void ApiClient::fetchBuffEquipment(const std::string& serverId,
                                    const std::string& characterId,
                                    std::function<void (const drogon::HttpResponsePtr& )> callback)
{
    std::string apiKey = ApiKeyManager::instance().getApiKey();
    std::string path = "/df/servers/" + serverId + "/characters/" + characterId + "/skill/buff/equip/equipment?apikey=" + apiKey;

    sendApiRequest(drogon::Get, path, callback);
}

void ApiClient::fetchBuffAvatar(const std::string& serverId,
                                const std::string& characterId,
                                std::function<void (const drogon::HttpResponsePtr& )> callback)
{
    std::string apiKey = ApiKeyManager::instance().getApiKey();
    std::string path = "/df/servers/" + serverId + "/characters/" + characterId + "/skill/buff/equip/avatar?apikey=" + apiKey;

    sendApiRequest(drogon::Get, path, callback);
}

void ApiClient::fetchBuffCreature(const std::string& serverId,
                                  const std::string& characterId,
                                  std::function<void (const drogon::HttpResponsePtr& )> callback)
{
    std::string apiKey = ApiKeyManager::instance().getApiKey();
    std::string path = "/df/servers/" + serverId + "/characters/" + characterId + "/skill/buff/equip/creature?apikey=" + apiKey;

    sendApiRequest(drogon::Get, path, callback);
}

void ApiClient::fetchTimeline(const std::string& serverId,
                              const std::string& characterId,
                              std::function<void (const drogon::HttpResponsePtr& )> callback,
                              int limit,
                              const std::string& code,
                              const std::string& startDate,
                              const std::string& endDate)
{
    std::string apiKey = ApiKeyManager::instance().getApiKey();
    std::string path = "/df/servers/" + serverId + "/characters/" + characterId + 
                        "/timeline?limit=" + std::to_string(limit);

    if(!code.empty()) path += "&code=" + code;
    if(!startDate.empty()) path += "&startDate=" + startDate;
    if(!endDate.empty()) path += "&endDate=" + endDate;

    path += "&apikey=" + apiKey;

    sendApiRequest(drogon::Get, path, callback);
}

void ApiClient::fetchTimelineNext(const std::string& serverId,
                                  const std::string& characterId,
                                  const std::string& nextToken,
                                  std::function<void (const drogon::HttpResponsePtr& )> callback)
{
    std::string apiKey = ApiKeyManager::instance().getApiKey();
    std::string path = "/df/servers/" + serverId + "/characters/" + characterId + 
                        "/timeline?next=" + nextToken + "&apikey=" + apiKey;

    sendApiRequest(drogon::Get, path, callback);
}