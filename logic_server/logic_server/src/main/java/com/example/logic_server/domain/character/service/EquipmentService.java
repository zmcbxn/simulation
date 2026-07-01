package com.example.logic_server.domain.character.service;

import com.example.logic_server.domain.character.dto.EquipmentDto;
import com.example.logic_server.domain.character.repository.EquipmentRepository;
import lombok.RequiredArgsConstructor;
import org.springframework.stereotype.Service;

import java.util.List;

@Service
@RequiredArgsConstructor
public class EquipmentService {

    private final EquipmentRepository equipmentRepository;

    public List<EquipmentDto> getEquipment(String serverId, String characterId) {
        return equipmentRepository.findByCharacterIdAndServerId(characterId, serverId)
                .stream()
                .map(EquipmentDto::fromEntity)
                .toList();
    }

}
