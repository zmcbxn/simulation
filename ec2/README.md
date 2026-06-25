# cal_site / ec2 (Comm 서버)

DNF(던전앤파이터) 캐릭터 정보 조회 및 스펙 시뮬레이션 서비스의 **Comm(Communication) 서버**.
Nexon 외부 API 데이터 수집, DB/Redis 저장, Logic 서버와의 Kafka 통신을 담당한다.

---

## 전체 시스템 아키텍처

```
[React Web]
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
| **Web (React)** | 캐릭터 검색 UI, 스펙 시뮬레이션 UI |
| **Logic 서버** | 비즈니스 판단, 데이터 가공, Web 응답 |
| **Comm 서버 (ec2, 이 저장소)** | Nexon API 호출, DB/Redis 저장, Kafka 발행 |

### Kafka 토픽

| 방향 | 토픽 | 내용 |
|------|------|------|
| Logic → Comm | (GW.ini `consume_topics`) | `processCharacterRequest` 등 요청 |
| Comm → Logic | `comm.to.logic` | `CHARACTER_READY` 완료 알림 |

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
│   └── ApiKeyManager               # API 키 로드/관리
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

## 핵심 흐름

### 1. 캐릭터 검색

```
Web → Logic → Kafka → CharacterService::processSearchCharacter
  └─ Nexon API fetchCharacter(name)
  └─ DB upsertBaseData (캐릭터 base 등록)
  └─ Redis HSETNX char:{id} update_time 0 (키 초기화)
  └─ 검색 결과 목록 반환
```

### 2. 캐릭터 상세 조회 (캐시 판단)

```
Web → Logic → Kafka → CharacterService::processCharacterRequest
  └─ Redis HGET char:{id} update_time
       ├─ 시간 이내 → FRESH 반환 (API 호출 없음)
       │    logicType == 1 (수동 새로고침): 쿨다운 60초
       │    logicType != 1 (자동, 검색 진입): 쿨다운 1800초
       └─ 만료 or 키 없음 → getFullApiFetch 호출
```

### 3. 전체 API 수집 (getFullApiFetch)

```
CharacterService::getFullApiFetch
  └─ ApiFetchContext 생성 (집계 객체)
  └─ 전체 타임아웃 30초 설정 (trantor::EventLoop::runAfter)
  └─ 11개 Nexon API 병렬 호출 (fetchWithRetry, 최대 3회 재시도)
       ├─ 능력치 / 장착장비 / 아바타 / 크리쳐 / 휘장
       ├─ 안개융화 / 스킬 / 버프강화장비 / 버프아바타 / 버프크리쳐 / 타임라인
  └─ 모든 API 완료 시 (All-or-Nothing)
       ├─ DB 저장 (saveToDatabase)
       ├─ Redis update_time 갱신
       └─ Kafka publish → comm.to.logic (CHARACTER_READY)
  └─ 하나라도 실패 시 → 전체 에러 반환 (나머지 결과 무시)
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

| 키 | 필드 | 내용 | 상태 |
|----|------|------|------|
| `char:{characterId}` | `update_time` | 마지막 API 수집 시각 (Unix timestamp) | 구현 완료 |
| `char:{characterId}` | 캐릭터 데이터 필드들 | 스펙 시뮬레이션용 캐릭터 정보 | **미구현** |

> Redis에 캐릭터 스펙 데이터를 저장하는 이유: 스펙 시뮬레이션은 DB 대신 Redis에서 빠른 read/write로 처리 예정.

---

## DB 저장 현황 (CharacterDAO)

| 메서드 | 테이블 | 상태 |
|--------|--------|------|
| `upsertBaseData` | `character.base_data` | 구현 완료 |
| `upsertStatusData` | `character.status_data` | 구현 완료 |
| `upsertEquipmentData` | `character.equipment_data` | 구현 완료 |
| `upsertAvatarData` | 미정 | **미구현** (CharacterService.cc 주석 처리) |
| `upsertCreatureData` | 미정 | **미구현** |
| `upsertFlagData` | 미정 | **미구현** |
| `upsertMistData` | 미정 | **미구현** |
| `upsertSkillData` | 미정 | **미구현** |
| `upsertBuffEquipData` | 미정 | **미구현** |
| `upsertBuffAvatarData` | 미정 | **미구현** |
| `upsertBuffCreatureData` | 미정 | **미구현** |
| `upsertTimelineData` | 미정 | **미구현** |

> `saveToDatabase`에 주석 처리된 DAO 호출이 있음. 각 DAO 구현 완료 후 주석 해제.

---

## 진행 현황

### 완료
- [x] Drogon 기반 Comm 서버 골격
- [x] Nexon API 11종 비동기 호출 (`ApiClient`)
- [x] 캐릭터 검색 / 캐시 판단 로직
- [x] `getFullApiFetch` 리팩토링
  - [x] `ApiFetchContext` 집계 객체
  - [x] `std::mutex`로 병렬 쓰기 Race Condition 방지
  - [x] `fetchWithRetry` (최대 3회 재시도)
  - [x] 전체 타임아웃 30초
  - [x] All-or-Nothing 완료/에러 확정 (`std::atomic<bool> done`)
  - [x] Kafka `CHARACTER_READY` 발행 (`publishCharacterReady`)
- [x] DB 저장: base / status / equipment
- [x] Kafka Consumer 핸들러 틀 (`KafkaManager::registerHandlers`)
- [x] Kafka Producer 초기화 및 `KafkaManager::getProducer()` 노출
- [x] `ServiceFactory` → `CharacterService`에 KafkaProducer 주입 (`main.cc`)

### 진행 중 / 예정
- [ ] Redis에 캐릭터 스펙 데이터 저장 (시뮬레이션용)
- [ ] DB 저장 나머지 항목 (avatar ~ timeline) DAO 구현 후 주석 해제
- [ ] 스펙 시뮬레이션 로직 (Logic 서버 또는 Comm 서버)
- [ ] Logic 서버 구현 (별도 저장소)
- [ ] Web 서버 (React) 구현 (별도 저장소)

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
```
