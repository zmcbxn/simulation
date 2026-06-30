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
                ? characterRepository.findAllByCharacterName(name)
                : characterRepository.findByServerIdAndCharacterName(serverId, name);

        if (!cached.isEmpty()) {
            sendResult(emitter, cached);
            return;
        }

        String correlationId = searchProducer.publishCharacterSearch(name, serverId);
        sseRegistry.register(correlationId, emitter);
    }

    public List<CharacterDto> searchByAdventureName(String adventureName) {
        return characterRepository.findByAdventureName(adventureName)
                .stream()
                .map(CharacterDto::fromEntity)
                .toList();
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
