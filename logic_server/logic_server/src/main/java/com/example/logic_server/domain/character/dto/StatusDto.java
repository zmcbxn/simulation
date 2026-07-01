package com.example.logic_server.domain.character.dto;

import lombok.Builder;
import lombok.Getter;

import java.util.List;

@Getter @Builder
public class StatusDto {

    private String characterId;
    private String serverId;
    private String characterName;
    private String adventureName;
    private String jobName;
    private String jobGrowName;
    private String guildName;
    private Integer level;
    private Integer fame;

    private Basic basic;
    private Core core;
    private Attack attack;
    private Key key;
    private BuffPower buffPower;
    private Element element;
    private Speed speed;
    private List<BuffEffect> buffEffects;

    @Getter @Builder
    public static class Basic {
        private Double hp;
        private Double mp;
    }

    @Getter @Builder
    public static class Core {
        private Double str;    // 힘
        private Double intel;  // 지능
        private Double vit;    // 체력
        private Double spr;    // 정신력
    }

    @Getter @Builder
    public static class Attack {
        private Double physicalAtk;     // 물리 공격
        private Double magicAtk;        // 마법 공격
        private Double independentAtk;  // 독립 공격
        private Double physicalCrit;    // 물리 크리티컬
        private Double magicCrit;       // 마법 크리티컬
    }

    @Getter @Builder
    public static class Key {
        private Double atkIncrease;       // 공격력 증가
        private Double atkAmplify;        // 공격력 증폭
        private Double finalDmgIncrease;  // 최종 데미지 증가
        private Double cdReduce;          // 쿨타임 감소
        private Double cdRecovery;        // 쿨타임 회복속도
        private Double finalCdReduce;     // 최종 쿨타임 감소율
    }

    @Getter @Builder
    public static class BuffPower {
        private Double buffPower;         // 버프력
        private Double buffPowerAmplify;  // 버프력 증폭
    }

    @Getter @Builder
    public static class ElementStat {
        private Double strengthen;  // 강화
        private Double resist;      // 저항
        private Double dmg;         // 피해
    }

    @Getter @Builder
    public static class Element {
        private ElementStat fire;   // 화
        private ElementStat water;  // 수
        private ElementStat light;  // 명
        private ElementStat dark;   // 암
    }

    @Getter @Builder
    public static class Speed {
        private Double attackSpeed;   // 공격 속도
        private Double castingSpeed;  // 캐스팅 속도
        private Double moveSpeed;     // 이동 속도
    }

    @Getter @Builder
    public static class BuffEffect {
        private String name;
        private Integer level;
        private List<StatEntry> status;
    }

    @Getter @Builder
    public static class StatEntry {
        private String name;
        private Double value;
    }
}
