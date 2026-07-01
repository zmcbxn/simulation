package com.example.logic_server.domain.character.controller;

import com.example.logic_server.domain.character.dto.EquipmentDto;
import com.example.logic_server.domain.character.dto.StatusDto;
import com.example.logic_server.domain.character.service.CharacterService;
import com.example.logic_server.domain.character.service.EquipmentService;
import com.example.logic_server.domain.character.service.StatusService;
import lombok.RequiredArgsConstructor;
import org.springframework.http.MediaType;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;
import org.springframework.web.servlet.mvc.method.annotation.SseEmitter;

import java.util.List;

@RestController
@RequestMapping("/api/character")
@RequiredArgsConstructor
public class CharacterController {

    private final CharacterService characterService;
    private final StatusService statusService;
    private final EquipmentService equipmentService;

    @GetMapping(value = "/{serverId}/{characterId}", produces = MediaType.TEXT_EVENT_STREAM_VALUE)
    public SseEmitter getCharacterDetail(
            @PathVariable String serverId,
            @PathVariable String characterId) {
        SseEmitter emitter = new SseEmitter(30_000L);
        characterService.getDetailWithSse(serverId, characterId, emitter);
        return emitter;
    }

    @GetMapping(value = "/{serverId}/{characterId}/refresh", produces = MediaType.TEXT_EVENT_STREAM_VALUE)
    public SseEmitter refreshCharacter(
            @PathVariable String serverId,
            @PathVariable String characterId) {
        SseEmitter emitter = new SseEmitter(30_000L);
        characterService.refreshWithSse(serverId, characterId, emitter);
        return emitter;
    }

    @GetMapping("/{serverId}/{characterId}/status")
    public ResponseEntity<StatusDto> getStatus(
            @PathVariable String serverId,
            @PathVariable String characterId) {
        return ResponseEntity.ok(statusService.getStatus(serverId, characterId));
    }

    @GetMapping("/{serverId}/{characterId}/equipment")
    public ResponseEntity<List<EquipmentDto>> getEquipment(
            @PathVariable String serverId,
            @PathVariable String characterId) {
        return ResponseEntity.ok(equipmentService.getEquipment(serverId, characterId));
    }

}
