package com.example.logic_server.domain.character.dto;

import com.example.logic_server.domain.character.entity.CharacterEntity;
import lombok.*;

@Getter
@Setter
@NoArgsConstructor
@AllArgsConstructor
@Builder
public class CharacterDto {
    private String characterId;
    private String serverId;
    private String characterName;
    private String adventureName;
    private String jobName;
    private String jobGrowName;
    private Integer level;
    private String guildName;
    private Integer fame;
    private String updatedDate;

    public static CharacterDto fromEntity(CharacterEntity entity) {
        return CharacterDto.builder()
                .characterId(entity.getCharacterId())
                .serverId(entity.getServerId())
                .characterName(entity.getCharacterName())
                .adventureName(entity.getAdventureName())
                .jobName(entity.getJobName())
                .jobGrowName(entity.getJobGrowName())
                .level(entity.getLevel())
                .guildName(entity.getGuildName())
                .fame(entity.getFame())
                .updatedDate(entity.getUpdatedDate().toString())
                .build();
    }
}
