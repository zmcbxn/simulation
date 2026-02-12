package com.example.logic_server.domain.character.entity;

import java.io.Serializable;
import lombok.*;

@Data
@NoArgsConstructor
@AllArgsConstructor
public class CharacterId implements Serializable{
    private String characterId;
    private String serverId;
}
