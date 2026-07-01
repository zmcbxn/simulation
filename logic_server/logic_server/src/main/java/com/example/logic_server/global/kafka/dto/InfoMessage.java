package com.example.logic_server.global.kafka.dto;

import lombok.Data;
import lombok.NoArgsConstructor;

import java.util.List;

@Data
@NoArgsConstructor
public class InfoMessage {
    private String correlationId;
    private String action;
    private String characterName;
    private List<String> serverList;
    private String characterId;
    private String serverId;
    private String reason;
    private Boolean refreshed;
}
