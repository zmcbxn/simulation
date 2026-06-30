package com.example.logic_server.domain.character.controller;

import com.example.logic_server.domain.character.dto.CharacterDto;
import com.example.logic_server.domain.character.service.CharacterService;
import com.example.logic_server.global.constant.ServerConstants;
import lombok.RequiredArgsConstructor;
import org.springframework.http.MediaType;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;
import org.springframework.web.servlet.mvc.method.annotation.SseEmitter;

import java.util.List;

@RestController
@RequiredArgsConstructor
@RequestMapping("/api/search")
public class SearchController {

    private final CharacterService characterService;

    // 모험단명 검색 — DB only, 즉시 반환
    @GetMapping("/adventureName/{name}")
    public ResponseEntity<List<CharacterDto>> searchByAdventureName(@PathVariable String name) {
        return ResponseEntity.ok(characterService.searchByAdventureName(name));
    }

    // SSE 검색 — DB 있으면 즉시, 없으면 Kafka → ec2 → SSE 이벤트
    @GetMapping(value = "/sse/{serverId}/{name}", produces = MediaType.TEXT_EVENT_STREAM_VALUE)
    public SseEmitter searchBySse(@PathVariable String serverId, @PathVariable String name) {
        SseEmitter emitter = new SseEmitter(30_000L);

        if (!ServerConstants.VALID_SERVER_IDS.contains(serverId) && !"all".equals(serverId)) {
            emitter.completeWithError(new IllegalArgumentException("Invalid serverId: " + serverId));
            return emitter;
        }

        characterService.searchWithSse(serverId, name, emitter);
        return emitter;
    }
}
