#include "MyHttpController.h"
#include "ApiClient.h"
#include "Character.h"

void MyHttpController::index(const HttpRequestPtr& req,
                             std::function<void (const HttpResponsePtr& )> &&callback)
{
    auto resp = HttpResponse::newHttpResponse();
    resp->setStatusCode(drogon::k200OK);
    resp->setContentTypeCode(drogon::CT_TEXT_HTML);
    
    resp->setBody(R"raw(
        <!DOCTYPE html>
        <html lang="ko">
        <head>
            <meta charset="UTF-8">
            <title>Drogon Character Search</title>
            <style>
                body { text-align: center; padding-top: 100px; font-family: sans-serif; background-color: #f8f9fa; }
                .container { max-width: 500px; margin: 0 auto; background: white; padding: 40px; border-radius: 15px; box-shadow: 0 4px 6px rgba(0,0,0,0.1); }
                h1 { color: #2c3e50; margin-bottom: 30px; }
                .search-box { display: flex; gap: 10px; justify-content: center; }
                input { padding: 12px 20px; font-size: 16px; border: 2px solid #ddd; border-radius: 8px; width: 250px; outline: none; }
                input:focus { border-color: #3498db; }
                button { padding: 12px 25px; font-size: 16px; background-color: #3498db; color: white; border: none; border-radius: 8px; cursor: pointer; }
                p { color: #7f8c8d; margin-top: 20px; font-size: 0.9em; }
            </style>
        </head>
        <body>
            <div class="container">
                <h1>🚀 Character Search</h1>
                <div class="search-box">
                    <input type="text" id="charName" placeholder="캐릭터명을 입력하세요" onkeypress="if(event.keyCode==13) searchCharacter()">
                    <button onclick="searchCharacter()">검색</button>
                </div>
                <p>결과는 /api/character/all/{이름} 경로로 이동하여 JSON으로 표시됩니다.</p>
            </div>

            <script>
                function searchCharacter() {
                    var charInput = document.getElementById("charName");
                    var name = charInput.value.trim();
                    
                    if (name === "") {
                        alert("캐릭터명을 입력해주세요!");
                        return;
                    }
                    window.location.href = "/api/character/all/" + encodeURIComponent(name);
                }
            </script>
        </body>
        </html>
    )raw");
    callback(resp);
}

void MyHttpController::getCharacter(const HttpRequestPtr& req,
                                    std::function<void (const HttpResponsePtr& )> &&callback,
                                    const std::string& serverId,
                                    const std::string& characterName)
{
    int logicType = 0;

    characterService.processCharacterRequest(serverId, characterName, logicType, [callback](const Json::Value& characterData) {
        Json::Value response;
        if(characterData.isMember("error")) {
            response["status"] = "Character not found";
            response["message"] = characterData["error"];
        } else {
            response["status"] = "success";
            response["data"] = characterData;
        }

        Json::StreamWriterBuilder builder;
        builder["commentStyle"] = "None";
        builder["indentation"] = "  "; // 이 설정은 들여쓰기를 2칸으로 만듭니다.
        builder["emitUTF8"] = true;
        std::string jsonString = Json::writeString(builder, response);

        auto resp = HttpResponse::newHttpResponse();
        resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);
        resp->setBody(jsonString);
        callback(resp);
    });
}

void MyHttpController::searchAll(const HttpRequestPtr& req,
                                 std::function<void (const HttpResponsePtr& )> &&callback,
                                 const std::string& name)
{
    characterService.processSearchCharacter(name, [callback](const Json::Value& characterData) {
        auto resp = HttpResponse::newHttpJsonResponse(characterData);
        callback(resp);
    });
}