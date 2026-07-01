package com.example.logic_server.domain.character.entity;

import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;

import java.io.Serializable;

@Data
@NoArgsConstructor
@AllArgsConstructor
public class EquipmentId implements Serializable {
    private String characterId;
    private String serverId;
    private String slotId;
}
