package com.example.logic_server.domain.character.repository;

import com.example.logic_server.domain.character.entity.CharacterEntity;
import com.example.logic_server.domain.character.entity.CharacterId;
import org.springframework.data.jpa.repository.JpaRepository;
import java.util.List;

public interface CharacterRepository extends JpaRepository<CharacterEntity, CharacterId> {
    List<CharacterEntity> findAllByCharacterName(String characterName);

    List<CharacterEntity> findByAdventureName(String adventureName);

    List<CharacterEntity> findByServerIdAndCharacterName(String serverId, String characterName);
}
