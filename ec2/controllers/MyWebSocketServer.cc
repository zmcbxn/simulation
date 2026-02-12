#include "MyWebSocketServer.h"
#include "CharacterService.h"
#include "ApiClient.h"
#include "Character.h"

void MyWebSocketServer::handleNewConnection(const HttpRequestPtr& req,
                                            const WebSocketConnectionPtr& wsConn)
{
    LOG_DEBUG << "New WebSocket connection established from "
              << req->getPeerAddr().toIpPort();
}

void MyWebSocketServer::handleNewMessage(const WebSocketConnectionPtr& wsConn, 
                                        std::string &&message, 
                                        const WebSocketMessageType& type){

    if(type != WebSocketMessageType::Text){
        LOG_DEBUG << "Unsupported message type received";
        return;
    }

    if(message.empty()){
        LOG_DEBUG << "Empty message received";
        return;
    }

    static CharacterService characterService;

    // 나중에 시뮬레이션 추가
}

void MyWebSocketServer::handleConnectionClosed(const WebSocketConnectionPtr& wsConn)
{
    LOG_DEBUG << "WebSocket connection closed: ";
}