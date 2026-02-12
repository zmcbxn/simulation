package com.example.logic_server.domain.character.dto;

import lombok.Getter;
import lombok.Setter;

@Getter @Setter
public class CharacterResponse<T> {
    private String status;
    private T data;
    private String message;
}
