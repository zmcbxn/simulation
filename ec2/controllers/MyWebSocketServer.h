#pragma once
#include <drogon/WebSocketController.h>

using namespace drogon;

class MyWebSocketServer : public drogon::WebSocketController<MyWebSocketServer>
{
    public:

        // 생성자와 소멸자
        MyWebSocketServer() = default;
        ~MyWebSocketServer() override = default;

        // 클라이언트가 메세지를 보냈을 때 호출되는 함수
        void handleNewMessage(const WebSocketConnectionPtr&, std::string &&, const WebSocketMessageType &) override;
        // 새로운 클라이언트가 접속했을 때 호출되는 함수
        void handleNewConnection(const HttpRequestPtr&, const WebSocketConnectionPtr&) override;
        // 클라이언트와의 연결이 종료되었을 때 호출되는 함수
        void handleConnectionClosed(const WebSocketConnectionPtr&) override;

        WS_PATH_LIST_BEGIN
        // 등록할 경로를 추가합니다. 예: WS_PATH_ADD("/chat");
        WS_PATH_ADD("/character");
        WS_PATH_LIST_END
};