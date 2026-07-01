package com.example.logic_server.global.kafka.consumer;

import com.example.logic_server.domain.character.dto.CharacterDetailDto;
import com.example.logic_server.domain.character.dto.CharacterDto;
import com.example.logic_server.domain.character.entity.CharacterEntity;
import com.example.logic_server.domain.character.repository.CharacterRepository;
import com.example.logic_server.domain.character.service.EquipmentService;
import com.example.logic_server.domain.character.service.StatusService;
import com.example.logic_server.global.kafka.dto.InfoMessage;
import com.example.logic_server.global.sse.SseRegistry;
import tools.jackson.databind.ObjectMapper;
import lombok.RequiredArgsConstructor;
import lombok.extern.slf4j.Slf4j;
import org.springframework.kafka.annotation.KafkaListener;
import org.springframework.stereotype.Component;
import org.springframework.web.servlet.mvc.method.annotation.SseEmitter;

import java.util.List;

@Slf4j
@Component
@RequiredArgsConstructor
public class InfoConsumer {

    private final SseRegistry sseRegistry;
    private final CharacterRepository characterRepository;
    private final StatusService statusService;
    private final EquipmentService equipmentService;
    private final ObjectMapper objectMapper;

    @KafkaListener(topics = "INFO", groupId = "logic-server-group")
    public void consume(String payload) {
        try {
            InfoMessage msg = objectMapper.readValue(payload, InfoMessage.class);
            switch (msg.getAction()) {
                case "character_search_ready"  -> handleSearchReady(msg);
                case "character_search_failed" -> handleFailed(msg);
                case "character_detail_ready"  -> handleDetailReady(msg);
                case "character_detail_failed" -> handleFailed(msg);
                default -> log.warn("[InfoConsumer] Unknown action: {}", msg.getAction());
            }
        } catch (Exception e) {
            log.error("[InfoConsumer] Parse error: {}", e.getMessage());
        }
    }

    private void handleSearchReady(InfoMessage msg) {
        SseEmitter emitter = sseRegistry.get(msg.getCorrelationId());
        if (emitter == null) return;

        try {
            List<CharacterEntity> entities = characterRepository
                    .findByCharacterNameIgnoreCaseAndServerIdIn(msg.getCharacterName(), msg.getServerList());
            List<CharacterDto> dtos = entities.stream().map(CharacterDto::fromEntity).toList();

            emitter.send(SseEmitter.event()
                    .name("result")
                    .data(objectMapper.writeValueAsString(dtos)));
            emitter.complete();
        } catch (Exception e) {
            emitter.completeWithError(e);
        } finally {
            sseRegistry.remove(msg.getCorrelationId());
        }
    }

    private void handleDetailReady(InfoMessage msg) {
        String correlationId = msg.getCorrelationId();
        SseEmitter emitter = sseRegistry.get(correlationId);
        if (emitter == null) return;

        try {
            if (correlationId.startsWith("R:")) {
                // 새로고침 요청
                if (Boolean.TRUE.equals(msg.getRefreshed())) {
                    emitter.send(SseEmitter.event()
                            .name("result")
                            .data("{\"refreshed\":true}"));
                } else {
                    long cooldown = statusService.getCooldownSeconds(msg.getServerId(), msg.getCharacterId());
                    emitter.send(SseEmitter.event()
                            .name("result")
                            .data("{\"refreshed\":false,\"cooldownSeconds\":" + cooldown + "}"));
                }
            } else {
                // 일반 상세 요청
                CharacterDetailDto detail = CharacterDetailDto.builder()
                        .status(statusService.getStatus(msg.getServerId(), msg.getCharacterId()))
                        .equipment(equipmentService.getEquipment(msg.getServerId(), msg.getCharacterId()))
                        .build();
                emitter.send(SseEmitter.event()
                        .name("result")
                        .data(objectMapper.writeValueAsString(detail)));
            }
            emitter.complete();
        } catch (Exception e) {
            log.error("[InfoConsumer] handleDetailReady error: {}", e.getMessage());
            emitter.completeWithError(e);
        } finally {
            sseRegistry.remove(correlationId);
        }
    }

    private void handleFailed(InfoMessage msg) {
        SseEmitter emitter = sseRegistry.get(msg.getCorrelationId());
        if (emitter == null) return;

        try {
            emitter.send(SseEmitter.event()
                    .name("error")
                    .data("{\"message\":\"" + msg.getReason() + "\"}"));
            emitter.complete();
        } catch (Exception e) {
            emitter.completeWithError(e);
        } finally {
            sseRegistry.remove(msg.getCorrelationId());
        }
    }
}
