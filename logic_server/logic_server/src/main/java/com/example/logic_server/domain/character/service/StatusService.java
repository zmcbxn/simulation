package com.example.logic_server.domain.character.service;

import com.example.logic_server.domain.character.dto.StatusDto;
import com.example.logic_server.domain.character.entity.CharacterId;
import com.example.logic_server.domain.character.entity.StatusEntity;
import com.example.logic_server.domain.character.repository.StatusRepository;
import lombok.RequiredArgsConstructor;
import lombok.extern.slf4j.Slf4j;
import org.springframework.stereotype.Service;
import tools.jackson.databind.JsonNode;
import tools.jackson.databind.ObjectMapper;

import java.time.Duration;
import java.time.LocalDateTime;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

@Slf4j
@Service
@RequiredArgsConstructor
public class StatusService {

    private final StatusRepository statusRepository;
    private final ObjectMapper objectMapper;

    private static final long REFRESH_COOLDOWN_SECONDS = 60L;

    public long getCooldownSeconds(String serverId, String characterId) {
        return statusRepository.findById(new CharacterId(characterId, serverId))
                .map(entity -> {
                    long elapsed = Duration.between(entity.getUpdatedDate(), LocalDateTime.now()).toSeconds();
                    return Math.max(0L, REFRESH_COOLDOWN_SECONDS - elapsed);
                })
                .orElse(0L);
    }

    public StatusDto getStatus(String serverId, String characterId) {
        StatusEntity entity = statusRepository.findById(new CharacterId(characterId, serverId))
                .orElseThrow(() -> new IllegalArgumentException("Status data not found"));
        return buildDto(entity);
    }

    private StatusDto buildDto(StatusEntity entity) {
        Map<String, Double> stats = new HashMap<>();
        List<StatusDto.BuffEffect> buffEffects = new ArrayList<>();
        String characterName = "";
        String jobGrowName = "";

        try {
            JsonNode root = objectMapper.readTree(entity.getRawData());

            characterName = root.path("characterName").asText("");
            jobGrowName   = root.path("jobGrowName").asText("");

            for (JsonNode s : root.path("status")) {
                stats.put(s.path("name").asText(), s.path("value").asDouble());
            }

            for (JsonNode b : root.path("buff")) {
                List<StatusDto.StatEntry> entries = new ArrayList<>();
                for (JsonNode s : b.path("status")) {
                    entries.add(StatusDto.StatEntry.builder()
                            .name(s.path("name").asText())
                            .value(s.path("value").asDouble())
                            .build());
                }
                int buffLevel = b.path("level").asInt(0);
                buffEffects.add(StatusDto.BuffEffect.builder()
                        .name(b.path("name").asText())
                        .level(buffLevel > 0 ? buffLevel : null)
                        .status(entries)
                        .build());
            }
        } catch (Exception e) {
            log.error("[StatusService] Failed to parse raw_data for {}/{}: {}", entity.getServerId(), entity.getCharacterId(), e.getMessage());
        }

        return StatusDto.builder()
                .characterId(entity.getCharacterId())
                .serverId(entity.getServerId())
                .characterName(characterName)
                .adventureName(entity.getAdventureName())
                .jobName(entity.getJobName())
                .jobGrowName(jobGrowName)
                .guildName(entity.getGuildName())
                .level(entity.getLevel())
                .fame(entity.getFame())
                .basic(StatusDto.Basic.builder()
                        .hp(stats.get("HP"))
                        .mp(stats.get("MP"))
                        .build())
                .core(StatusDto.Core.builder()
                        .str(stats.get("힘"))
                        .intel(stats.get("지능"))
                        .vit(stats.get("체력"))
                        .spr(stats.get("정신력"))
                        .build())
                .attack(StatusDto.Attack.builder()
                        .physicalAtk(stats.get("물리 공격"))
                        .magicAtk(stats.get("마법 공격"))
                        .independentAtk(stats.get("독립 공격"))
                        .physicalCrit(stats.get("물리 크리티컬"))
                        .magicCrit(stats.get("마법 크리티컬"))
                        .build())
                .key(StatusDto.Key.builder()
                        .atkIncrease(stats.get("공격력 증가"))
                        .atkAmplify(stats.get("공격력 증폭"))
                        .finalDmgIncrease(stats.get("최종 데미지 증가"))
                        .cdReduce(stats.get("쿨타임 감소"))
                        .cdRecovery(stats.get("쿨타임 회복속도"))
                        .finalCdReduce(stats.get("최종 쿨타임 감소율"))
                        .build())
                .buffPower(StatusDto.BuffPower.builder()
                        .buffPower(stats.get("버프력"))
                        .buffPowerAmplify(stats.get("버프력 증폭"))
                        .build())
                .element(StatusDto.Element.builder()
                        .fire(StatusDto.ElementStat.builder()
                                .strengthen(stats.get("화속성 강화"))
                                .resist(stats.get("화속성 저항"))
                                .dmg(stats.get("화속성 피해"))
                                .build())
                        .water(StatusDto.ElementStat.builder()
                                .strengthen(stats.get("수속성 강화"))
                                .resist(stats.get("수속성 저항"))
                                .dmg(stats.get("수속성 피해"))
                                .build())
                        .light(StatusDto.ElementStat.builder()
                                .strengthen(stats.get("명속성 강화"))
                                .resist(stats.get("명속성 저항"))
                                .dmg(stats.get("명속성 피해"))
                                .build())
                        .dark(StatusDto.ElementStat.builder()
                                .strengthen(stats.get("암속성 강화"))
                                .resist(stats.get("암속성 저항"))
                                .dmg(stats.get("암속성 피해"))
                                .build())
                        .build())
                .speed(StatusDto.Speed.builder()
                        .attackSpeed(stats.get("공격 속도"))
                        .castingSpeed(stats.get("캐스팅 속도"))
                        .moveSpeed(stats.get("이동 속도"))
                        .build())
                .buffEffects(buffEffects)
                .build();
    }
}
