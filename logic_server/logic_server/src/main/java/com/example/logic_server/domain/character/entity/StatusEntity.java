package com.example.logic_server.domain.character.entity;

import jakarta.persistence.*;
import lombok.Getter;
import lombok.NoArgsConstructor;
import org.hibernate.annotations.Formula;
import java.time.LocalDateTime;

@Entity
@Table(name = "status_data", schema = "character")
@IdClass(CharacterId.class)
@Getter @NoArgsConstructor
public class StatusEntity {

    @Id
    @Column(name = "character_id")
    private String characterId;

    @Id
    @Column(name = "server_id")
    private String serverId;

    @Column(name = "level")
    private Integer level;

    @Column(name = "job_name")
    private String jobName;

    @Column(name = "adventure_name")
    private String adventureName;

    @Column(name = "fame")
    private Integer fame;

    @Column(name = "guild_name")
    private String guildName;

    @Formula("CAST(raw_data AS TEXT)")
    private String rawData;

    @Column(name = "updated_date")
    private LocalDateTime updatedDate;
}
