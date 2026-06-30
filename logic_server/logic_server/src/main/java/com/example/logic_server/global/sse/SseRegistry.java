package com.example.logic_server.global.sse;

import org.springframework.stereotype.Component;
import org.springframework.web.servlet.mvc.method.annotation.SseEmitter;

import java.util.concurrent.ConcurrentHashMap;

@Component
public class SseRegistry {

    private final ConcurrentHashMap<String, SseEmitter> emitters = new ConcurrentHashMap<>();

    public void register(String correlationId, SseEmitter emitter) {
        emitters.put(correlationId, emitter);
        emitter.onCompletion(() -> emitters.remove(correlationId));
        emitter.onTimeout(() -> emitters.remove(correlationId));
        emitter.onError(e -> emitters.remove(correlationId));
    }

    public SseEmitter get(String correlationId) {
        return emitters.get(correlationId);
    }

    public void remove(String correlationId) {
        emitters.remove(correlationId);
    }
}
