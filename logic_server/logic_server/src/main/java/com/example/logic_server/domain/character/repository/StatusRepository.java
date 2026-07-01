package com.example.logic_server.domain.character.repository;

import com.example.logic_server.domain.character.entity.CharacterId;
import com.example.logic_server.domain.character.entity.StatusEntity;
import org.springframework.data.jpa.repository.JpaRepository;

public interface StatusRepository extends JpaRepository<StatusEntity, CharacterId> {
}
