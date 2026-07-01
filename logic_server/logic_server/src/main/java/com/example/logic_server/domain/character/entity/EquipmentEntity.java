package com.example.logic_server.domain.character.entity;

import jakarta.persistence.*;
import lombok.Getter;
import lombok.NoArgsConstructor;
import org.hibernate.annotations.Formula;

@Entity
@Table(name = "equipment_data", schema = "character")
@IdClass(EquipmentId.class)
@Getter @NoArgsConstructor
public class EquipmentEntity {

    @Id
    @Column(name = "character_id")
    private String characterId;

    @Id
    @Column(name = "server_id")
    private String serverId;

    @Id
    @Column(name = "slot_id")
    private String slotId;

    @Column(name = "slot_name")
    private String slotName;

    @Column(name = "item_id", nullable = false)
    private String itemId;

    @Column(name = "item_name")
    private String itemName;

    @Column(name = "item_rarity")
    private String itemRarity;

    @Column(name = "item_grade_name")
    private String itemGradeName;

    @Column(name = "reinforce_value")
    private Integer reinforceValue;

    @Column(name = "is_amplified")
    private Boolean isAmplified;

    @Column(name = "amplification_name")
    private String amplificationName;

    @Column(name = "refine_value")
    private Integer refineValue;

    @Formula("CAST(upgrade_info AS TEXT)")
    private String upgradeInfo;
}
