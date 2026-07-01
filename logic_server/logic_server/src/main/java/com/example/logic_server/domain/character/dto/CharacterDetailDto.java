package com.example.logic_server.domain.character.dto;

import lombok.Builder;
import lombok.Getter;

import java.util.List;

@Getter @Builder
public class CharacterDetailDto {
    private StatusDto status;
    private List<EquipmentDto> equipment;
}