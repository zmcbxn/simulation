package com.example.logic_server.domain.character.dto;

import com.example.logic_server.domain.character.entity.CharacterEntity;
import lombok.*;

import java.time.OffsetDateTime;

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
    private OffsetDateTime updatedDate;

    public static CharacterDto fromEntity(CharacterEntity entity) {
        return CharacterDto.builder()
                .characterId(entity.getCharacterId())
                .serverId(entity.getServerId())
                .characterName(entity.getCharacterName())
                .jobName(entity.getJobName())
                .jobGrowName(entity.getJobGrowName())
                .level(entity.getLevel())
                .updatedDate(entity.getUpdatedDate())
                .build();
    }
}