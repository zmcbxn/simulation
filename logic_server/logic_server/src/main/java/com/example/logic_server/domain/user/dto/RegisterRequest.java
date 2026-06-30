package com.example.logic_server.domain.user.dto;

import lombok.Data;

@Data
public class RegisterRequest {
    private String username;
    private String password;
    private String apiKey;
}