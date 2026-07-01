# cal_site / logic_server

DNF(던전앤파이터) 캐릭터 정보 조회 및 스펙 시뮬레이션 서비스의 **Logic 서버**.
Web 클라이언트 요청 처리, 비즈니스 로직, Kafka를 통한 Comm 서버 연동을 담당한다.

---

## 전체 시스템 아키텍처

```
[Web 클라이언트 - React/Vite :3000]
    │  HTTP REST (Axios) + SSE (EventSource)
    ▼
[Logic 서버 - Java/Spring Boot :8081]  ◄──── Kafka ────► [Comm 서버 (ec2) - C++/Drogon]
    │                                                              │
    │  JPA (PostgreSQL)                                    ┌───────┴───────┐
    ▼                                                      ▼               ▼
[PostgreSQL]  ◄────────────────────────────────────── [PostgreSQL]     [Redis]
 users 스키마                                          character 스키마  (캐시)
 (회원 정보)                                           (캐릭터 데이터)
```

### 서버별 역할

| 서버 | 역할 |
|------|------|
| **Web (React)** | UI, 사용자 인터랙션 |
| **Logic 서버 (이 저장소)** | 비즈니스 로직, 인증/세션, Web 응답 |
| **Comm 서버 (ec2, C++)** | Nexon API 호출, DB/Redis 저장, Kafka 발행 |

### Kafka 토픽

| 방향 | 토픽 | action | 내용 |
|------|------|--------|------|
| Logic → Comm | `SEARCH` | `character_search` | 캐릭터 검색 요청 |
| Logic → Comm | `SEARCH` | `character_detail` | 캐릭터 상세 요청 (type:0 일반 / type:1 강제 새로고침) |
| Comm → Logic | `INFO` | `character_search_ready` | 검색 완료, serverList 포함 |
| Comm → Logic | `INFO` | `character_search_failed` | 검색 실패, reason 포함 |
| Comm → Logic | `INFO` | `character_detail_ready` | 상세 수집 완료, refreshed(true/false) 포함 |
| Comm → Logic | `INFO` | `character_detail_failed` | 상세 수집 실패 |

#### character_detail 메시지 형식 (Logic → Comm)

```json
{
  "correlationId": "D:uuid 또는 R:uuid",
  "action": "character_detail",
  "serverId": "prey",
  "characterId": "06329c853f...",
  "type": 0
}
```

- `type: 0` — 일반 로드 (ec2가 freshness 판단, 60초 이상 경과 시 자동 재조회)
- `type: 1` — 강제 새로고침 (무조건 Nexon API 재호출)
- correlationId prefix: `D:` = 일반 상세, `R:` = 새로고침 요청

#### character_detail_ready 메시지 형식 (Comm → Logic)

```json
{
  "correlationId": "D:uuid 또는 R:uuid",
  "action": "character_detail_ready",
  "serverId": "prey",
  "characterId": "06329c853f...",
  "refreshed": true
}
```

---

## 기술 스택

| 항목 | 기술 |
|------|------|
| 프레임워크 | Spring Boot 4.0.2 (Java 21) |
| DB | PostgreSQL (`users` 스키마) + Spring Data JPA |
| 캐시 | Redis (Spring Session) |
| 메시지 큐 | Apache Kafka (Spring Kafka) |
| 인증 | Spring Security + Spring Session Redis (세션 기반) |
| 보안 | BCrypt 비밀번호 암호화 |
| 빌드 | Gradle |

---

## 디렉터리 구조

```
src/main/java/com/example/logic_server/
├── domain/
│   ├── character/
│   │   ├── controller/
│   │   │   ├── SearchController             # SSE 검색 엔드포인트
│   │   │   └── CharacterController          # 캐릭터 상세 SSE + 새로고침 + 개별 탭
│   │   ├── dto/
│   │   │   ├── CharacterDto                 # 검색 결과 카드용
│   │   │   ├── CharacterDetailDto           # SSE 상세 응답 (status + equipment)
│   │   │   ├── StatusDto                    # 능력치 (Basic/Core/Attack/Key/BuffPower/Element/Speed/BuffEffect)
│   │   │   └── EquipmentDto                 # 장비 슬롯 1개 (itemId, itemImageUrl 포함)
│   │   ├── entity/
│   │   │   ├── CharacterEntity, CharacterId # base_data
│   │   │   ├── StatusEntity                 # status_data (raw_data → @Formula TEXT)
│   │   │   └── EquipmentEntity, EquipmentId # equipment_data (upgrade_info → @Formula TEXT)
│   │   ├── repository/
│   │   │   ├── CharacterRepository          # ILIKE 대소문자 무시 검색
│   │   │   ├── StatusRepository
│   │   │   └── EquipmentRepository
│   │   └── service/
│   │       ├── CharacterService             # DB-first 검색 + 상세/새로고침 Kafka 발행
│   │       ├── StatusService                # raw_data JSON 파싱 → StatusDto, 쿨다운 계산
│   │       └── EquipmentService             # 전체 슬롯 조회
│   ├── user/
│   │   ├── controller/AuthController        # 인증 엔드포인트
│   │   ├── dto/LoginRequest, RegisterRequest, UserResponse
│   │   ├── entity/MemberEntity
│   │   ├── repository/MemberRepository
│   │   └── service/MemberService            # BCrypt + 회원가입/로그인
│   ├── simulation/
│   │   └── service/SimService               # 장비 시뮬레이션 (미구현)
│   └── timeline/
│       └── repository/TimelineRepository    # 타임라인 (미구현)
└── global/
    ├── config/
    │   ├── KafkaConfig                      # SEARCH/INFO 토픽 선언
    │   └── SecurityConfig                   # Spring Security 설정
    ├── constant/
    │   └── ServerConstants                  # 유효 서버ID 목록
    ├── exception/
    │   └── GlobalExceptionHandler           # 공통 에러 응답
    ├── kafka/
    │   ├── consumer/InfoConsumer            # INFO 토픽 수신 → SSE 라우팅
    │   ├── producer/SearchProducer          # SEARCH 토픽 발행
    │   └── dto/SearchMessage, InfoMessage
    ├── sse/
    │   └── SseRegistry                      # correlationId → SseEmitter 매핑
    └── security/
        └── UserDetailsServiceImpl
```

---

## HTTP API

### 인증 `/api/auth`

| 메서드 | 경로 | 설명 | 상태 |
|--------|------|------|------|
| `POST` | `/api/auth/register` | 회원가입 (username, password, apiKey) | ✅ 완료 |
| `POST` | `/api/auth/login` | 로그인 → 세션 생성 | ✅ 완료 |
| `POST` | `/api/auth/logout` | 로그아웃 → 세션 파기 | ✅ 완료 |
| `GET`  | `/api/auth/me` | 현재 로그인 유저 정보 | ✅ 완료 |

### 캐릭터 검색 `/api/search`

| 메서드 | 경로 | 설명 | 상태 |
|--------|------|------|------|
| `GET` | `/api/search/sse/{serverId}/{name}` | SSE 캐릭터 검색 — 대소문자 무시 | ✅ 완료 |
| `GET` | `/api/search/adventureName/{name}` | 모험단명 검색 (DB only) — 대소문자 무시 | ✅ 완료 |

### 캐릭터 상세 `/api/character`

| 메서드 | 경로 | 설명 | 상태 |
|--------|------|------|------|
| `GET` (SSE) | `/api/character/{serverId}/{characterId}` | 캐릭터 상세 (status + equipment) | ✅ 완료 |
| `GET` (SSE) | `/api/character/{serverId}/{characterId}/refresh` | 강제 새로고침 요청 | ✅ 완료 |
| `GET` | `/api/character/{serverId}/{characterId}/status` | 능력치만 조회 (DB direct) | ✅ 완료 |
| `GET` | `/api/character/{serverId}/{characterId}/equipment` | 장비 전체(13슬롯) 조회 (DB direct) | ✅ 완료 |

#### SSE 상세 응답 형식 (`CharacterDetailDto`)

```json
{
  "status": {
    "characterId": "...",
    "serverId": "...",
    "characterName": "...",
    "adventureName": "...",
    "jobName": "...",
    "jobGrowName": "...",
    "guildName": "...",
    "level": 110,
    "fame": 12345,
    "basic":     { "hp": 0.0, "mp": 0.0 },
    "core":      { "str": 0.0, "intel": 0.0, "vit": 0.0, "spr": 0.0 },
    "attack":    { "physicalAtk": 0.0, "magicAtk": 0.0, "independentAtk": 0.0, "physicalCrit": 0.0, "magicCrit": 0.0 },
    "key":       { "atkIncrease": 0.0, "atkAmplify": 0.0, "finalDmgIncrease": 0.0, "cdReduce": 0.0, "cdRecovery": 0.0, "finalCdReduce": 0.0 },
    "buffPower": { "buffPower": 0.0, "buffPowerAmplify": 0.0 },
    "element": {
      "fire":  { "strengthen": 0.0, "resist": 0.0, "dmg": 0.0 },
      "water": { "strengthen": 0.0, "resist": 0.0, "dmg": 0.0 },
      "light": { "strengthen": 0.0, "resist": 0.0, "dmg": 0.0 },
      "dark":  { "strengthen": 0.0, "resist": 0.0, "dmg": 0.0 }
    },
    "speed":     { "attackSpeed": 0.0, "castingSpeed": 0.0, "moveSpeed": 0.0 },
    "buffEffects": [
      { "name": "...", "level": 1, "status": [{ "name": "...", "value": 0.0 }] }
    ]
  },
  "equipment": [
    {
      "slotId": "WEAPON",
      "slotName": "무기",
      "itemId": "3d6a68d3...",
      "itemName": "...",
      "itemRarity": "에픽",
      "itemGradeName": "...",
      "reinforceValue": 12,
      "isAmplified": true,
      "amplificationName": "특상",
      "refineValue": 0,
      "upgradeInfo": { ... }
    }
  ]
}
```

> 아이템 이미지 URL: `https://img-api.neople.co.kr/df/items/{itemId}`

#### SSE 새로고침 응답 형식

```json
// refreshed: true → 프론트에서 일반 상세 엔드포인트 재호출
{ "refreshed": true }

// refreshed: false → 쿨다운 남은 시간 반환 (60초 기준)
{ "refreshed": false, "cooldownSeconds": 42 }
```

### 시뮬레이션 `/api/simulation`

| 메서드 | 경로 | 설명 | 상태 |
|--------|------|------|------|
| `POST` | `/api/simulation/{serverId}/{characterId}` | 장비 교체 후 스펙 계산 | ❌ 미구현 |

### 랭킹 `/api/ranking`

| 메서드 | 경로 | 설명 | 상태 |
|--------|------|------|------|
| `GET` | `/api/ranking` | 직업군별 스펙 랭킹 | ❌ 미구현 |

### 타임라인 `/api/timeline`

| 메서드 | 경로 | 설명 | 상태 |
|--------|------|------|------|
| `GET` | `/api/timeline/{serverId}/{characterId}` | 캐릭터 활동 타임라인 | ❌ 미구현 |

### 경매장 `/api/auction`

| 메서드 | 경로 | 설명 | 상태 |
|--------|------|------|------|
| `GET` | `/api/auction/search` | 아이템 검색 (`?itemName=`) | ❌ 미구현 |

---

## DB 스키마

### `users` 스키마 (Logic 서버 관할)

| 테이블 | 설명 |
|--------|------|
| `users.member` | 회원 정보 (id, username, password BCrypt, apiKey, created_at, updated_at) |

### `character` 스키마 (Comm 서버 관할, Logic 서버는 읽기만)

| 테이블 | 주요 컬럼 | 설명 |
|--------|-----------|------|
| `character.base_data` | character_id, server_id, character_name, job_name, job_grow_name, level, updated_date | 캐릭터 기본 정보 |
| `character.status_data` | character_id, server_id, level, job_name, adventure_name, fame, guild_name, raw_data(jsonb), updated_date | 능력치 (raw_data 파싱) |
| `character.equipment_data` | character_id, server_id, slot_id, slot_name, item_id, item_name, item_rarity, item_grade_name, reinforce_value, is_amplified, amplification_name, refine_value, upgrade_info(jsonb) | 장착 장비 (13슬롯) |
| `character.avatar_data` | - | 아바타 |
| `character.creature_data` | - | 크리쳐 |
| `character.oath_data` | - | 서약 |
| `character.skill_data` | - | 스킬 |
| `character.buff_equip_data` | - | 버프 장비 |
| `character.buff_avatar_data` | - | 버프 아바타 |
| `character.buff_creature_data` | - | 버프 크리쳐 |
| `character.timeline_data` | - | 타임라인 |

#### jsonb 컬럼 매핑 방식

Hibernate + Jackson 3.x(`tools.jackson`) 환경에서 `@JdbcTypeCode(SqlTypes.JSON)`이 동작하지 않는 문제로,
jsonb 컬럼은 `@Formula("CAST(column AS TEXT)")` 로 TEXT로 읽어 서비스 레이어에서 직접 파싱한다.

---

## 캐릭터 검색 흐름

```
Web → GET /api/search/sse/{serverId}/{name}  (SSE, EventSource)
  └─ CharacterService
       ├─ DB 조회 — ILIKE 대소문자 무시
       │    ├─ 있음 → SSE event "result" 즉시 반환
       │    └─ 없음 → SEARCH 토픽 발행 (character_search)
       │              └─ Comm 서버: Nexon API 호출 → DB 저장
       │                  └─ INFO 토픽 (character_search_ready)
       │                      └─ InfoConsumer → SSE event "result"
       └─ Web: result/error 이벤트 수신 → 카드 표시
```

## 캐릭터 상세 흐름

```
Web → GET /api/character/{serverId}/{characterId}  (SSE, EventSource, 30초 타임아웃)
  └─ CharacterService.getDetailWithSse()  [correlationId: "D:uuid"]
       └─ SEARCH 토픽 발행 (character_detail, type:0)
          └─ Comm 서버: freshness 판단 → 필요 시 Nexon API 재조회
              └─ INFO 토픽 (character_detail_ready, refreshed:true/false)
                  └─ InfoConsumer.handleDetailReady() — "D:" prefix 감지
                      ├─ StatusService.getStatus()       → StatusDto
                      ├─ EquipmentService.getEquipment() → List<EquipmentDto>
                      └─ SSE event "result": CharacterDetailDto JSON
```

## 새로고침 흐름

```
Web → GET /api/character/{serverId}/{characterId}/refresh  (SSE, EventSource, 30초 타임아웃)
  └─ CharacterService.refreshWithSse()  [correlationId: "R:uuid"]
       └─ SEARCH 토픽 발행 (character_detail, type:1)
          └─ Comm 서버: 무조건 Nexon API 재조회 or 쿨다운(60초) 판단
              └─ INFO 토픽 (character_detail_ready, refreshed:true/false)
                  └─ InfoConsumer.handleDetailReady() — "R:" prefix 감지
                      ├─ refreshed:true  → SSE event { refreshed: true }
                      │                    → 프론트에서 상세 엔드포인트 재호출
                      └─ refreshed:false → StatusService.getCooldownSeconds()
                                          → SSE event { refreshed: false, cooldownSeconds: N }
```

---

## 진행 현황

### 완료
- [x] Spring Boot 프로젝트 세팅 (Java 21, Gradle)
- [x] PostgreSQL JPA 연동
- [x] Redis 연동 (Spring Session)
- [x] Kafka Producer/Consumer (SEARCH/INFO 토픽)
- [x] SseRegistry (correlationId → SseEmitter 비동기 라우팅)
- [x] 회원가입 / 로그인 / 로그아웃 / 세션 복원
- [x] Spring Security 설정
- [x] 캐릭터 검색 SSE — ILIKE 대소문자 무시
- [x] 모험단명 검색 — ILIKE 대소문자 무시
- [x] 캐릭터 상세 SSE (`/api/character/{serverId}/{characterId}`)
- [x] 강제 새로고침 SSE (`/api/character/{serverId}/{characterId}/refresh`)
- [x] StatusEntity / StatusService / StatusDto (raw_data jsonb 파싱)
- [x] EquipmentEntity / EquipmentService / EquipmentDto (itemId 포함, 13슬롯)
- [x] InfoConsumer — correlationId prefix(D:/R:)로 상세/새로고침 분기
- [x] 쿨다운 계산 (StatusService.getCooldownSeconds, 60초 기준)

### 진행 예정
- [ ] 나머지 탭 엔드포인트 (on-demand DB 조회)
  - [ ] `/skill`, `/avatar`, `/creature`, `/oath`, 버프 탭
- [ ] 장비 upgradeInfo JSON 파싱 (현재 raw 전달)
- [ ] 장비 시뮬레이션
- [ ] 랭킹
- [ ] 타임라인
- [ ] 경매장
- [ ] 마이페이지

---

## 빌드 및 실행

```bash
./gradlew build -x test
./gradlew bootRun
```

> 서버 포트: **8081** | PostgreSQL: `127.0.0.1:5432/pgdb` | Redis: `127.0.0.1:6379` | Kafka: `127.0.0.1:9092`

---

## 개발 원칙

- **DB-first 검색**: 캐릭터 검색은 DB 먼저, 없을 때만 Comm 서버 요청
- **Lazy tab loading**: 상세는 메인 SSE로 status+equipment 먼저, 나머지 탭은 on-demand
- **세션 기반 인증**: Spring Session + Redis, 타임아웃 1800초
- **스키마 분리**: `users`(회원) / `character`(캐릭터) 스키마 분리
- **jsonb 처리**: `@Formula("CAST(col AS TEXT)")` 패턴으로 Hibernate/Jackson 3.x 호환성 확보
- **대소문자 무시 검색**: `IgnoreCase` / `lower()` 사용
