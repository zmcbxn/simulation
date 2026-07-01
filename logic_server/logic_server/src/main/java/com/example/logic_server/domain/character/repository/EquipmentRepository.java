package com.example.logic_server.domain.character.repository;

import com.example.logic_server.domain.character.entity.EquipmentEntity;
import com.example.logic_server.domain.character.entity.EquipmentId;
import org.springframework.data.jpa.repository.JpaRepository;

import java.util.List;

public interface EquipmentRepository extends JpaRepository<EquipmentEntity, EquipmentId> {
    List<EquipmentEntity> findByCharacterIdAndServerId(String characterId, String serverId);
}
