package com.example.logic_server.global.kafka.consumer;


import com.example.logic_server.domain.character.dto.CharacterMessage;
import org.springframework.stereotype.Component;
import org.springframework.kafka.annotation.KafkaListener;

@Component
public class CharacterConsumer {
    @KafkaListener(topics = "MESSAGE", groupId = "logic-server-group")
    public void consumeMessage(CharacterMessage message) {
        if ("COMPLETE".equals(message.getType())) {
            System.out.println("[Kafka] 수집 완료 메시지 수신: " + message.getCharacterName());
        }
    }
}
