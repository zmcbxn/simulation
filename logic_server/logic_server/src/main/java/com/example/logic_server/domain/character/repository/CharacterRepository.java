package com.example.logic_server.domain.character.repository;

import com.example.logic_server.domain.character.dto.CharacterDto;
import com.example.logic_server.domain.character.entity.CharacterEntity;
import com.example.logic_server.domain.character.entity.CharacterId;
import com.example.logic_server.domain.character.entity.StatusEntity;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.data.jpa.repository.Query;
import org.springframework.data.repository.query.Param;

import java.util.List;

public interface CharacterRepository extends JpaRepository<CharacterEntity, CharacterId> {
    List<CharacterEntity> findAllByCharacterNameIgnoreCase(String characterName);
    List<CharacterEntity> findByServerIdAndCharacterNameIgnoreCase(String serverId, String characterName);
    List<CharacterEntity> findByCharacterNameIgnoreCaseAndServerIdIn(String characterName, List<String> serverIds);

    @Query("SELECT new com.example.logic_server.domain.character.dto.CharacterDto(" +
           "b.characterId, b.serverId, b.characterName, s.adventureName, b.jobName, b.jobGrowName, b.level, b.updatedDate) " +
           "FROM CharacterEntity b, StatusEntity s " +
           "WHERE b.characterId = s.characterId AND b.serverId = s.serverId AND lower(s.adventureName) = lower(:adventureName)")
    List<CharacterDto> findByAdventureName(@Param("adventureName") String adventureName);
}
