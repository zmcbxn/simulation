package com.example.logic_server.domain.character.service;

import com.example.logic_server.domain.character.dto.CharacterDto;
import com.example.logic_server.domain.character.entity.CharacterEntity;
import com.example.logic_server.domain.character.repository.CharacterRepository;
import com.example.logic_server.global.kafka.producer.CharacterProducer;
import lombok.RequiredArgsConstructor;
import org.springframework.stereotype.Service;
import java.util.List;
import java.util.stream.Collectors;

@Service
@RequiredArgsConstructor
public class CharacterService {
    private final CharacterRepository characterRepository;
    private final CharacterProducer characterProducer;

    public List<CharacterDto> searchAllByCharacterName(String characterName) {
        List<CharacterEntity> character = characterRepository.findAllByCharacterName(characterName);
        if(character.isEmpty()) {
            characterProducer.produceCharacter("all", characterName);
        }
        return character.stream()
                .map(CharacterDto::fromEntity)
                .collect(Collectors.toList());
    }

    public List<CharacterDto> searchByServerIdAndCharacterName(String serverId, String characterName) {
        List<CharacterEntity> character = characterRepository.findByServerIdAndCharacterName(serverId, characterName);
        if(character.isEmpty()) {
            characterProducer.produceCharacter(serverId, characterName);
        }

        return character.stream()
                .map(CharacterDto::fromEntity)
                .collect(Collectors.toList());
    }

    public List<CharacterDto> searchByAdventureName(String adventureName) {
        return characterRepository.findByAdventureName(adventureName)
                .stream()
                .map(CharacterDto::fromEntity)
                .collect(Collectors.toList());
    }
}
