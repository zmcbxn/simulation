#pragma once

#include <drogon/HttpController.h>
#include "services/CharacterService.h"
#define BASE_PATH ""

using namespace drogon;

class MyHttpController : public drogon::HttpController<MyHttpController>
{
  public:
    MyHttpController() = default;
    ~MyHttpController() override = default;

  
    METHOD_LIST_BEGIN
      ADD_METHOD_TO(MyHttpController::index, "/", Get);
      // use METHOD_ADD to add your custom processing function here;
      // METHOD_ADD(MyHttpController::get, "/{2}/{1}", Get); // path is /MyHttpController/{arg2}/{arg1}
      // METHOD_ADD(MyHttpController::your_method_name, "/{1}/{2}/list", Get); // path is /MyHttpController/{arg1}/{arg2}/list
      // ADD_METHOD_TO(MyHttpController::your_method_name, "/absolute/path/{1}/{2}/list", Get); // path is /absolute/path/{arg1}/{arg2}/list
      ADD_METHOD_TO(MyHttpController::searchAll, "/api/character/all/{characterName}", Get);
      ADD_METHOD_TO(MyHttpController::getCharacter, "/api/character/{serverId}/{characterName}", Get);
    METHOD_LIST_END

    // your declaration of processing function maybe like this:
    // void get(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback, int p1, std::string p2);
    // void your_method_name(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback, double p1, int p2) const;
    void index(const drogon::HttpRequestPtr& req,
               std::function<void (const drogon::HttpResponsePtr& )>&& callback);
    
    void getCharacter(const HttpRequestPtr& req, 
                      std::function<void (const HttpResponsePtr& )> &&callback,
                      const std::string& serverId,
                      const std::string& characterName);

    void searchAll(const HttpRequestPtr& req, 
                  std::function<void (const HttpResponsePtr& )> &&callback,
                  const std::string& name);

    private:
      CharacterService characterService;
};
