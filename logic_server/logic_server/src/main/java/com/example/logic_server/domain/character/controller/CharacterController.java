package com.example.logic_server.domain.character.controller;

import com.example.logic_server.domain.character.dto.CharacterDto;
import com.example.logic_server.domain.character.service.CharacterService;
import lombok.RequiredArgsConstructor;
import org.springframework.web.bind.annotation.*;
import java.util.List;

@RestController
@RequiredArgsConstructor
@RequestMapping("/api/character")
public class CharacterController {
    private final CharacterService characterService;

    @GetMapping("/all/{characterName}")
    public List<CharacterDto> searchAllByCharacterName(@PathVariable String characterName) {
        return characterService.searchAllByCharacterName(characterName);
    }

    @GetMapping("/adventure/{adventureName}")
    public List<CharacterDto> searchByAdventureName(@PathVariable String adventureName) {
        return characterService.searchByAdventureName(adventureName);
    }

    @GetMapping("/{serverId}/{characterName}")
    public List<CharacterDto> searchByServerIdAndCharacterName(@PathVariable String serverId, @PathVariable String characterName) {
        return characterService.searchByServerIdAndCharacterName(serverId, characterName);
    }
}

