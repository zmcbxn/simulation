package com.example.logic_server.domain.character.service;

import com.example.logic_server.domain.character.dto.CharacterDto;
import com.example.logic_server.domain.character.entity.CharacterEntity;
import com.example.logic_server.domain.character.repository.CharacterRepository;
import com.example.logic_server.global.kafka.producer.SearchProducer;
import com.example.logic_server.global.sse.SseRegistry;
import tools.jackson.databind.ObjectMapper;
import lombok.RequiredArgsConstructor;
import lombok.extern.slf4j.Slf4j;
import org.springframework.stereotype.Service;
import org.springframework.web.servlet.mvc.method.annotation.SseEmitter;

import java.util.List;
import java.util.UUID;

@Slf4j
@Service
@RequiredArgsConstructor
public class CharacterService {

    private final CharacterRepository characterRepository;
    private final SearchProducer searchProducer;
    private final SseRegistry sseRegistry;
    private final ObjectMapper objectMapper;

    public void searchWithSse(String serverId, String name, SseEmitter emitter) {
        List<CharacterEntity> cached = "all".equals(serverId)
                ? characterRepository.findAllByCharacterNameIgnoreCase(name)
                : characterRepository.findByServerIdAndCharacterNameIgnoreCase(serverId, name);

        if (!cached.isEmpty()) {
            sendResult(emitter, cached);
            return;
        }

        String correlationId = searchProducer.publishCharacterSearch(name, serverId);
        sseRegistry.register(correlationId, emitter);
    }

    public List<CharacterDto> searchByAdventureName(String adventureName) {
        return characterRepository.findByAdventureName(adventureName);
    }

    public void getDetailWithSse(String serverId, String characterId, SseEmitter emitter) {
        String correlationId = "D:" + UUID.randomUUID();
        searchProducer.publishCharacterDetail(correlationId, serverId, characterId, 0);
        emitter.onTimeout(() -> {
            sseRegistry.remove(correlationId);
            try {
                emitter.send(SseEmitter.event().name("error").data("{\"message\":\"요청 시간이 초과되었습니다.\"}"));
            } catch (Exception ignored) {}
            emitter.complete();
        });
        sseRegistry.register(correlationId, emitter);
    }

    public void refreshWithSse(String serverId, String characterId, SseEmitter emitter) {
        String correlationId = "R:" + UUID.randomUUID();
        searchProducer.publishCharacterDetail(correlationId, serverId, characterId, 1);
        emitter.onTimeout(() -> {
            sseRegistry.remove(correlationId);
            try {
                emitter.send(SseEmitter.event().name("error").data("{\"message\":\"요청 시간이 초과되었습니다.\"}"));
            } catch (Exception ignored) {}
            emitter.complete();
        });
        sseRegistry.register(correlationId, emitter);
    }

    private void sendResult(SseEmitter emitter, List<CharacterEntity> entities) {
        try {
            List<CharacterDto> dtos = entities.stream().map(CharacterDto::fromEntity).toList();
            emitter.send(SseEmitter.event()
                    .name("result")
                    .data(objectMapper.writeValueAsString(dtos)));
            emitter.complete();
        } catch (Exception e) {
            emitter.completeWithError(e);
        }
    }
}
