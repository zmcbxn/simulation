package com.example.logic_server.global.kafka.dto;

import com.fasterxml.jackson.annotation.JsonInclude;
import lombok.Builder;
import lombok.Getter;

@Getter
@Builder
@JsonInclude(JsonInclude.Include.NON_NULL)
public class SearchMessage {
    private String correlationId;
    private String action;
    private String characterName;
    private String serverId;
    private String characterId;
    private Integer type;
}
