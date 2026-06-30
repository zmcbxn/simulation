# cal_site / ec2 (Comm 서버)

DNF(던전앤파이터) 캐릭터 정보 조회 및 스펙 시뮬레이션 서비스의 **Comm(Communication) 서버**.
Nexon 외부 API 데이터 수집, DB/Redis 저장, Logic 서버와의 Kafka 통신을 담당한다.

---

## 전체 시스템 아키텍처

```
[Web 서버 (Java/Spring)]
    │  HTTP
    ▼
[Logic 서버]  ◄──── Kafka ────► [Comm 서버 / ec2]
                                      │
                               ┌──────┴───────┐
                               ▼              ▼
                          [PostgreSQL]     [Redis]
                          (영구 저장)   (캐시 / 시뮬레이션)
                               ▲
                         [Nexon Open API]
```

### 서버별 역할

| 서버 | 역할 |
|------|------|
| **Web (Java/Spring)** | 캐릭터 검색 UI, 스펙 시뮬레이션 UI |
| **Logic 서버** | 비즈니스 판단, 데이터 가공, Web 응답 |
| **Comm 서버 (ec2, 이 저장소)** | Nexon API 호출, DB/Redis 저장, Kafka 발행 |

### Kafka 토픽 및 액션 명세

**[Java → ec2] 토픽: `SEARCH`**

| action | 설명 | 상태 |
|--------|------|------|
| `character_search` | 닉네임으로 전 서버 목록 검색 | 구현 완료 |
| `character_detail` | 특정 캐릭터 상세 수집 (11종 API) | 구현 완료 |
| `timeline` | 타임라인 조회 | 미구현 |
| `auction` | 경매장 검색 | 미구현 |

메시지 형식:
```json
// character_search
{ "correlationId": "uuid", "action": "character_search", "characterName": "더스크워치" }

// character_detail
{ "correlationId": "uuid", "action": "character_detail", "serverId": "prey", "characterName": "더스크워치", "type": 0 }
```
> `correlationId`: Logic 서버가 생성한 UUID. INFO 응답 매칭용  
> `type`: 0 = 자동(쿨다운 1800초), 1 = 수동 새로고침(쿨다운 60초)

---

**[ec2 → Java] 토픽: `INFO`**

| action | 설명 | 상태 |
|--------|------|------|
| `character_search_ready` | 목록 반환 | 구현 완료 |
| `character_search_failed` | 목록 검색 실패 | 구현 완료 |
| `character_detail_ready` | 상세 수집 완료 | 구현 완료 |
| `character_detail_failed` | 상세 수집 실패 | 구현 완료 |
| `timeline_ready` | 타임라인 반환 | 미구현 |
| `timeline_failed` | 타임라인 실패 | 미구현 |
| `auction_ready` | 경매장 결과 반환 | 미구현 |
| `auction_failed` | 경매장 실패 | 미구현 |

메시지 형식:
```json
// character_search_ready
{ "correlationId": "uuid", "action": "character_search_ready", "characterName": "더스크워치", "serverList": ["prey", "sirocco", "cain"] }

// character_search_failed
{ "correlationId": "uuid", "action": "character_search_failed", "characterName": "더스크워치", "reason": "..." }

// character_detail_ready
{ "correlationId": "uuid", "action": "character_detail_ready", "characterId": "f0fa67...", "serverId": "prey" }

// character_detail_failed
{ "correlationId": "uuid", "action": "character_detail_failed", "characterId": "f0fa67...", "serverId": "prey", "reason": "API fetch timed out" }
```

---

## 디렉터리 구조

```
ec2/
├── main.cc                         # 진입점, Kafka/Drogon 초기화
├── config/settings.json            # Drogon 설정 (DB, Redis 연결)
├── ini/
│   ├── GW.ini                      # Kafka 브로커/토픽 설정
│   └── apikeys.txt                 # Nexon API 키
│
├── common/
│   ├── ApiKeyManager               # API 키 로드/관리
│   └── jsoncpp_sv_shim.cc          # jsoncpp string_view 호환 shim (dlsym)
│
├── controllers/
│   ├── RestController              # HTTP REST 엔드포인트 (Web → Comm 직접 호출용)
│   ├── MyHttpController            # 기타 HTTP
│   └── MyWebSocketServer           # WebSocket (미사용/예정)
│
├── services/
│   ├── ApiClient                   # Nexon Open API HTTP 호출 래퍼
│   ├── CharacterService            # 핵심 비즈니스 로직 (검색 / 조회 / 수집)
│   ├── ServiceFactory              # Lazy Singleton 팩토리
│   └── kafka/
│       ├── KafkaConfig             # GW.ini 파싱
│       ├── KafkaConsumer           # 토픽 구독 및 핸들러 디스패치
│       ├── KafkaProducer           # 메시지 발행
│       └── KafkaManager            # Consumer/Producer 통합 관리
│
├── dao/
│   └── CharacterDAO                # PostgreSQL CRUD (execSqlAsync 비동기)
│
└── models/
    └── Character.h                 # 캐릭터 집계 데이터 구조체 + toJson()
```

---

## HTTP API 엔드포인트

| 메서드 | 경로 | 설명 |
|--------|------|------|
| `GET` | `/api/character/all/{name}` | 닉네임으로 전 서버 캐릭터 목록 검색 |
| `GET` | `/api/character/{server}/{name}` | 특정 서버 캐릭터 상세 조회 (11종 API 수집) |

---

## 핵심 흐름

### 1. 캐릭터 검색 (`/api/character/all/{name}`)

```
RestController → CharacterService::processSearchCharacter
  └─ Nexon API fetchCharacter(name)  ← 항상 Nexon API 호출
  └─ DB upsertBaseData (캐릭터 base 등록/갱신)
  └─ 검색 결과 목록 반환

※ 추후 개선 예정: DB에 이미 있으면 DB에서 반환, 없으면 Nexon API 호출
```

### 2. 캐릭터 상세 조회 (`/api/character/{server}/{name}`)

```
RestController → CharacterService::processCharacterRequest
  └─ Redis HGET char:{characterId} update_time
       ├─ 1800초 이내 → FRESH 반환 (Nexon API 호출 없음)
       │    logicType == 1 (수동 새로고침): 쿨다운 60초
       └─ 만료 or 키 없음 → getFullApiFetch 호출
```

### 3. 전체 API 수집 (`getFullApiFetch`)

```
CharacterService::getFullApiFetch
  └─ ApiFetchContext 생성 (집계 객체)
  └─ 전체 타임아웃 30초 (trantor::EventLoop::runAfter)
  └─ 11개 Nexon API 병렬 호출 (fetchWithRetry, 최대 3회 재시도)
       ├─ 능력치 / 장착장비 / 아바타 / 크리쳐
       ├─ 서약 / 안개융화 / 스킬
       └─ 버프장비 / 버프아바타 / 버프크리쳐 / 타임라인
  └─ 동일 캐릭터 중복 요청 시 fetch 1회만 실행 (inFlight 맵)
       └─ 대기 중인 correlationId 전체에 결과 발행
  └─ 모든 API 완료 시 (All-or-Nothing)
       ├─ DB 저장 (saveToDatabase → 12개 테이블)
       ├─ Redis update_time 갱신 + TTL 7일 설정
       └─ Kafka INFO 토픽 → character_detail_ready 발행
  └─ 하나라도 실패 시 → character_detail_failed 발행
```

---

## 기술 스택

| 항목 | 기술 |
|------|------|
| 서버 프레임워크 | [Drogon](https://github.com/drogonframework/drogon) (C++17/20, 비동기 논블로킹) |
| DB | PostgreSQL (`drogon::orm::DbClient` 비동기) |
| 캐시 | Redis (`drogon::nosql::RedisClient` 비동기) |
| 메시지 큐 | Apache Kafka (cppkafka / librdkafka) |
| 외부 API | Nexon Open API (DNF) |
| 빌드 | CMake |

---

## Redis 사용 현황

| 키 | 필드 | 내용 | TTL |
|----|------|------|-----|
| `char:{characterId}` | `update_time` | 마지막 API 수집 시각 (Unix timestamp) | 7일 (갱신 시 리셋) |
| `char:{characterId}` | 캐릭터 데이터 필드들 | 스펙 시뮬레이션용 캐릭터 정보 | **미구현** |

---

## DB 저장 현황 (CharacterDAO)

| 메서드 | 테이블 | 저장 내용 |
|--------|--------|-----------|
| `upsertBaseData` | `character.base_data` | 캐릭터 기본 정보 (이름/서버/직업/레벨/명성) |
| `upsertStatusData` | `character.status_data` | 능력치 raw_data + 요약 |
| `upsertEquipmentData` | `character.equipment_data` | 장착 장비 슬롯별 (강화/증폭/봉인/인챈트/융합/튠) |
| `upsertOathData` | `character.oath_data` | 서약 본체 + 묵언의 진의 단계 + raw_data |
| `upsertAvatarData` | `character.avatar_data` | 아바타 슬롯별 (아이템/옵션/클론/엠블렘) |
| `upsertCreatureData` | `character.creature_data` | 크리쳐 + 클론 + 아티팩트 |
| `upsertMistData` | `character.mist_data` | 안개융화 레벨/경험치/raw_data |
| `upsertSkillData` | `character.skill_data` | 스킬 해시 + 액티브/패시브 + raw_data |
| `upsertBuffEquipData` | `character.buff_equip_data` | 버프 강화 장비 슬롯별 |
| `upsertBuffAvatarData` | `character.buff_avatar_data` | 버프 아바타 슬롯별 + 엠블렘 유무 |
| `upsertBuffCreatureData` | `character.buff_creature_data` | 버프 크리쳐 |
| `upsertTimelineData` | `character.timeline_data` | 최근 30일 타임라인 이벤트 |

---

## 진행 현황

### 완료

- [x] Drogon 기반 Comm 서버 골격
- [x] Nexon API 11종 비동기 호출 (`ApiClient`)
  - 능력치 / 장착장비 / 아바타 / 크리쳐 / 서약 / 안개융화 / 스킬 / 버프장비 / 버프아바타 / 버프크리쳐 / 타임라인
  - ~~휘장(flag)~~ → Nexon API에서 완전 삭제됨
- [x] 캐릭터 검색 / 캐시 판단 로직 (update_time 기반 1800초/60초)
- [x] `getFullApiFetch` 구현
  - `ApiFetchContext` 집계 객체
  - `std::mutex`로 병렬 쓰기 Race Condition 방지
  - `fetchWithRetry` (최대 3회 재시도)
  - 전체 타임아웃 30초
  - All-or-Nothing 완료/에러 확정 (`std::atomic<bool> done`)
- [x] DB 저장 전 테이블 구현 (CharacterDAO 12종)
- [x] Redis `update_time` 관리 + TTL 7일
- [x] Kafka SEARCH/INFO 토픽 연동 (Logic 서버 실제 통신 검증 완료)
  - `character_search_ready/failed`, `character_detail_ready/failed` 발행
  - `correlationId` 기반 요청-응답 매칭
  - KafkaConsumer fields 필터 제거 (수신 메시지 전체 파싱)
- [x] 동일 캐릭터 중복 fetch 방지 (`inFlight` 맵 + mutex)
- [x] jsoncpp string_view 무한재귀 버그 수정 (dlsym shim)

### 예정

- [ ] Redis 캐릭터 스펙 데이터 저장 (로직 서버 설계 후 진행)
- [ ] 검색 DB-first: DB에 있으면 Nexon API 호출 생략
- [ ] FRESH 시 DB 데이터 반환 (현재는 `{"status":"FRESH"}` 만 반환)
- [ ] timeline, auction Kafka 핸들러 구현
- [ ] 유저/인증 시스템

---

## 개발 원칙

- **비동기 논블로킹**: 모든 I/O는 콜백 기반 (`execSqlAsync`, `execCommandAsync`, `HttpClient`)
- **Lazy Singleton**: 서비스 인스턴스는 `ServiceFactory::getCharacterService()`를 통해서만 접근
- **All-or-Nothing**: 11개 API 중 하나라도 최종 실패 시 DB/Redis 저장하지 않음

---

## 빌드

```bash
mkdir build && cd build
cmake ..
make -j$(nproc)
./ec2
```

> 실행 디렉터리에 `apikeys.txt`, `GW.ini` 가 복사되어 있어야 한다 (CMake configure_file로 자동 처리).
