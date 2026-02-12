#include "RestController.h"

void RestController::getCharacterStatus(const HttpRequestPtr& req,
                                        std::function<void (const HttpResponsePtr& )> &&callback)
{
    Json::Value response;
    response["status"] = "success";
    response["data"]["service"] = "Character Status";
    response["data"]["connection"] = "active";

    auto resp = HttpResponse::newHttpJsonResponse(response);
    callback(resp);
}