package com.example.logic_server.domain.character.dto;

import com.example.logic_server.domain.character.entity.EquipmentEntity;
import lombok.Builder;
import lombok.Getter;

@Getter @Builder
public class EquipmentDto {

    private String slotId;
    private String slotName;
    private String itemId;
    private String itemName;
    private String itemRarity;
    private String itemGradeName;
    private Integer reinforceValue;
    private Boolean isAmplified;
    private String amplificationName;
    private Integer refineValue;
    private Object upgradeInfo;

    public static EquipmentDto fromEntity(EquipmentEntity e) {
        return EquipmentDto.builder()
                .slotId(e.getSlotId())
                .slotName(e.getSlotName())
                .itemId(e.getItemId())
                .itemName(e.getItemName())
                .itemRarity(e.getItemRarity())
                .itemGradeName(e.getItemGradeName())
                .reinforceValue(e.getReinforceValue())
                .isAmplified(e.getIsAmplified())
                .amplificationName(e.getAmplificationName())
                .refineValue(e.getRefineValue())
                .upgradeInfo(e.getUpgradeInfo())
                .build();
    }
}
