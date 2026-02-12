package com.example.logic_server.domain.character.dto;

import lombok.Data;
import lombok.AllArgsConstructor;
import lombok.NoArgsConstructor;

@Data @AllArgsConstructor @NoArgsConstructor
public class CharacterMessage {
    private String serverId;
    private String characterName;
    private String type;
}
