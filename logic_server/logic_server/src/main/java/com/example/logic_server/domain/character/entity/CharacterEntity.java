package com.example.logic_server.domain.character.entity;

import jakarta.persistence.*;
import lombok.Getter;
import lombok.NoArgsConstructor;
import lombok.Setter;

import java.time.OffsetDateTime;

@Entity
@Table(name = "base_data", schema = "character")
@IdClass(CharacterId.class)
@Getter @Setter @NoArgsConstructor
public class CharacterEntity {

    @Id
    @Column(name = "character_id")
    private String characterId;

    @Id
    @Column(name = "server_id", nullable = false)
    private String serverId;

    @Column(name = "character_name", nullable = false)
    private String characterName;

    @Column(name = "adventure_name")
    private String adventureName;

    @Column(name = "job_name")
    private String jobName;

    @Column(name = "job_grow_name")
    private String jobGrowName;

    @Column(name = "level")
    private Integer level;

    @Column(name = "updated_date")
    private OffsetDateTime updatedDate;
}
