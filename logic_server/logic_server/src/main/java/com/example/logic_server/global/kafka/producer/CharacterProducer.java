package com.example.logic_server.global.kafka.producer;

import com.example.logic_server.domain.character.dto.CharacterMessage;
import lombok.RequiredArgsConstructor;
import org.springframework.stereotype.Component;
import org.springframework.kafka.core.KafkaTemplate;

@Component
@RequiredArgsConstructor
public class CharacterProducer {
    private final KafkaTemplate<String, Object> kafkaTemplate;
    private static final String TOPIC = "CHARACTER";

    public void produceCharacter(String serverId, String characterName){
        CharacterMessage message = new CharacterMessage(serverId, characterName, "REQUEST");
        kafkaTemplate.send(TOPIC, message);
    }
}
