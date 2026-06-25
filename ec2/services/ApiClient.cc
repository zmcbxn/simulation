#include "ApiClient.h"
#include "../common/ApiKeyManager.h"

void ApiClient::sendApiRequest(const std::string& userId,
                                drogon::HttpMethod method,
                                const std::string& basePath,
                                const std::function<void(const drogon::HttpResponsePtr&)>& callback)
{
    ApiKeyManager::instance().getApiKey(userId, [=](const std::string& key) {
        if (key.empty()) {
            callback(nullptr);
            return;
        }
        auto client = drogon::HttpClient::newHttpClient("https://api.neople.co.kr");
        auto req = drogon::HttpRequest::newHttpRequest();
        req->setMethod(method);
        req->setPath(basePath + "&apikey=" + key);
        client->sendRequest(req, [callback](drogon::ReqResult result, const drogon::HttpResponsePtr& resp) {
            callback(result == drogon::ReqResult::Ok ? resp : nullptr);
        });
    });
}

void ApiClient::fetchCharacter(const std::string& characterName,
                                const std::string& userId,
                                std::function<void(const drogon::HttpResponsePtr&)> callback)
{
    std::string path = "/df/servers/all/characters?characterName=" + characterName;
    sendApiRequest(userId, drogon::Get, path, callback);
}

void ApiClient::fetchCharacterStatus(const std::string& serverId,
                                      const std::string& characterId,
                                      const std::string& userId,
                                      std::function<void(const drogon::HttpResponsePtr&)> callback)
{
    std::string path = "/df/servers/" + serverId + "/characters/" + characterId + "/status?1";
    sendApiRequest(userId, drogon::Get, path, callback);
}

void ApiClient::fetchCharacterEquipment(const std::string& serverId,
                                         const std::string& characterId,
                                         const std::string& userId,
                                         std::function<void(const drogon::HttpResponsePtr&)> callback)
{
    std::string path = "/df/servers/" + serverId + "/characters/" + characterId + "/equip/equipment?1";
    sendApiRequest(userId, drogon::Get, path, callback);
}

void ApiClient::fetchAvatar(const std::string& serverId,
                             const std::string& characterId,
                             const std::string& userId,
                             std::function<void(const drogon::HttpResponsePtr&)> callback)
{
    std::string path = "/df/servers/" + serverId + "/characters/" + characterId + "/equip/avatar?1";
    sendApiRequest(userId, drogon::Get, path, callback);
}

void ApiClient::fetchCreature(const std::string& serverId,
                               const std::string& characterId,
                               const std::string& userId,
                               std::function<void(const drogon::HttpResponsePtr&)> callback)
{
    std::string path = "/df/servers/" + serverId + "/characters/" + characterId + "/equip/creature?1";
    sendApiRequest(userId, drogon::Get, path, callback);
}

void ApiClient::fetchOath(const std::string& serverId,
                           const std::string& characterId,
                           const std::string& userId,
                           std::function<void(const drogon::HttpResponsePtr&)> callback)
{
    std::string path = "/df/servers/" + serverId + "/characters/" + characterId + "/equip/oath?1";
    sendApiRequest(userId, drogon::Get, path, callback);
}

void ApiClient::fetchMist(const std::string& serverId,
                           const std::string& characterId,
                           const std::string& userId,
                           std::function<void(const drogon::HttpResponsePtr&)> callback)
{
    std::string path = "/df/servers/" + serverId + "/characters/" + characterId + "/equip/mist-assimilation?1";
    sendApiRequest(userId, drogon::Get, path, callback);
}

void ApiClient::fetchSkills(const std::string& serverId,
                             const std::string& characterId,
                             const std::string& userId,
                             std::function<void(const drogon::HttpResponsePtr&)> callback)
{
    std::string path = "/df/servers/" + serverId + "/characters/" + characterId + "/skill/style?1";
    sendApiRequest(userId, drogon::Get, path, callback);
}

void ApiClient::fetchBuffEquipment(const std::string& serverId,
                                    const std::string& characterId,
                                    const std::string& userId,
                                    std::function<void(const drogon::HttpResponsePtr&)> callback)
{
    std::string path = "/df/servers/" + serverId + "/characters/" + characterId + "/skill/buff/equip/equipment?1";
    sendApiRequest(userId, drogon::Get, path, callback);
}

void ApiClient::fetchBuffAvatar(const std::string& serverId,
                                 const std::string& characterId,
                                 const std::string& userId,
                                 std::function<void(const drogon::HttpResponsePtr&)> callback)
{
    std::string path = "/df/servers/" + serverId + "/characters/" + characterId + "/skill/buff/equip/avatar?1";
    sendApiRequest(userId, drogon::Get, path, callback);
}

void ApiClient::fetchBuffCreature(const std::string& serverId,
                                   const std::string& characterId,
                                   const std::string& userId,
                                   std::function<void(const drogon::HttpResponsePtr&)> callback)
{
    std::string path = "/df/servers/" + serverId + "/characters/" + characterId + "/skill/buff/equip/creature?1";
    sendApiRequest(userId, drogon::Get, path, callback);
}

void ApiClient::fetchTimeline(const std::string& serverId,
                               const std::string& characterId,
                               const std::string& userId,
                               std::function<void(const drogon::HttpResponsePtr&)> callback,
                               int limit,
                               const std::string& code,
                               const std::string& startDate,
                               const std::string& endDate)
{
    std::string path = "/df/servers/" + serverId + "/characters/" + characterId +
                       "/timeline?limit=" + std::to_string(limit);
    if (!code.empty())      path += "&code=" + code;
    if (!startDate.empty()) path += "&startDate=" + startDate;
    if (!endDate.empty())   path += "&endDate=" + endDate;
    sendApiRequest(userId, drogon::Get, path, callback);
}

void ApiClient::fetchTimelineNext(const std::string& serverId,
                                   const std::string& characterId,
                                   const std::string& userId,
                                   const std::string& nextToken,
                                   std::function<void(const drogon::HttpResponsePtr&)> callback)
{
    std::string path = "/df/servers/" + serverId + "/characters/" + characterId +
                       "/timeline?next=" + nextToken;
    sendApiRequest(userId, drogon::Get, path, callback);
}
