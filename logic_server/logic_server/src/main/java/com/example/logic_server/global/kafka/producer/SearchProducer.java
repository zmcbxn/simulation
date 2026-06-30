package com.example.logic_server.global.kafka.producer;

import com.example.logic_server.global.kafka.dto.SearchMessage;
import lombok.RequiredArgsConstructor;
import lombok.extern.slf4j.Slf4j;
import org.springframework.kafka.core.KafkaTemplate;
import org.springframework.stereotype.Component;
import tools.jackson.databind.ObjectMapper;

import java.util.UUID;

@Slf4j
@Component
@RequiredArgsConstructor
public class SearchProducer {

    private final KafkaTemplate<String, String> kafkaTemplate;
    private final ObjectMapper objectMapper;
    private static final String TOPIC = "SEARCH";

    public String publishCharacterSearch(String characterName, String serverId) {
        String correlationId = UUID.randomUUID().toString();
        SearchMessage msg = SearchMessage.builder()
                .correlationId(correlationId)
                .action("character_search")
                .characterName(characterName)
                .serverId(serverId)
                .build();
        send(msg);
        return correlationId;
    }

    public String publishCharacterDetail(String serverId, String characterId, int type) {
        String correlationId = UUID.randomUUID().toString();
        SearchMessage msg = SearchMessage.builder()
                .correlationId(correlationId)
                .action("character_detail")
                .serverId(serverId)
                .characterId(characterId)
                .type(type)
                .build();
        send(msg);
        return correlationId;
    }

    private void send(SearchMessage msg) {
        try {
            kafkaTemplate.send(TOPIC, objectMapper.writeValueAsString(msg));
        } catch (Exception e) {
            log.error("[SearchProducer] send error: {}", e.getMessage());
        }
    }
}
