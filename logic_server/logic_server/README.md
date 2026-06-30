# cal_site / logic_server

DNF(던전앤파이터) 캐릭터 정보 조회 및 스펙 시뮬레이션 서비스의 **Logic 서버**.
Web 클라이언트 요청 처리, 비즈니스 로직, Kafka를 통한 Comm 서버 연동을 담당한다.

---

## 전체 시스템 아키텍처

```
[Web 클라이언트 - React/Vite :3000]
    │  HTTP REST (Axios)
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
| Logic → Comm | `SEARCH` | `character_detail` | 캐릭터 상세 요청 |
| Comm → Logic | `INFO` | `character_search_ready` | 검색 완료, serverList 포함 |
| Comm → Logic | `INFO` | `character_search_failed` | 검색 실패, reason 포함 |
| Comm → Logic | `INFO` | `character_detail_ready` | 상세 수집 완료 |
| Comm → Logic | `INFO` | `character_detail_failed` | 상세 수집 실패 |

---

## 기술 스택

| 항목 | 기술 |
|------|------|
| 프레임워크 | Spring Boot 4.0.2 (Java 21) |
| DB | PostgreSQL (`users` 스키마) + Spring Data JPA |
| 캐시 | Redis (Spring Session + 추후 캐릭터 데이터 캐시) |
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
│   │   │   └── CharacterController          # 캐릭터 상세 (미구현)
│   │   ├── dto/CharacterDto
│   │   ├── entity/CharacterEntity, CharacterId
│   │   ├── repository/CharacterRepository
│   │   └── service/CharacterService         # DB-first 조회 + Kafka 발행
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
| `GET` | `/api/search/sse/{serverId}/{name}` | SSE 캐릭터 검색 (all 또는 서버ID) | ✅ 완료 |
| `GET` | `/api/search/adventureName/{name}` | 모험단명 검색 (DB only) | ✅ 완료 |

### 캐릭터 상세 `/api/character`

| 메서드 | 경로 | 설명 | 상태 |
|--------|------|------|------|
| `GET` | `/api/character/{serverId}/{characterId}` | 캐릭터 상세 정보 | ❌ 미구현 |

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

| 테이블 | 설명 |
|--------|------|
| `character.base_data` | 캐릭터 기본 정보 — Logic 서버 JPA Entity 매핑됨 |
| `character.status_data` | 능력치 |
| `character.equipment_data` | 장착 장비 |
| `character.avatar_data` | 아바타 |
| `character.creature_data` | 크리쳐 |
| `character.oath_data` | 서약 |
| `character.mist_data` | 안개융화 |
| `character.skill_data` | 스킬 |
| `character.buff_equip_data` | 버프 장비 |
| `character.buff_avatar_data` | 버프 아바타 |
| `character.buff_creature_data` | 버프 크리쳐 |
| `character.timeline_data` | 타임라인 |

---

## 캐릭터 검색 흐름

```
Web → GET /api/search/sse/{serverId}/{name}  (SSE 연결)
  └─ CharacterService
       ├─ DB 조회 (character.base_data)
       │    ├─ 있음 → SSE event "result" 즉시 반환
       │    └─ 없음 → SearchProducer: SEARCH 토픽 발행 (character_search)
       │              correlationId → SseRegistry 등록 (연결 유지)
       │              └─ Comm 서버: Nexon API 호출 → DB 저장
       │                  └─ INFO 토픽 발행 (character_search_ready / failed)
       │                      └─ InfoConsumer: SseRegistry에서 emitter 조회
       │                          └─ SSE event "result" 또는 "error" 전송
       └─ Web: EventSource로 result/error 이벤트 수신 → 카드 표시
```

---

## 진행 현황

### 완료
- [x] Spring Boot 프로젝트 세팅 (Java 21, Gradle)
- [x] PostgreSQL JPA 연동 (`character.base_data` Entity)
- [x] Redis 연동 (Spring Session)
- [x] Kafka Producer/Consumer (SEARCH/INFO 토픽)
- [x] SseRegistry (correlationId → SseEmitter 비동기 라우팅)
- [x] 회원가입 / 로그인 / 로그아웃 / 세션 복원
- [x] Spring Security 설정 (BCrypt, 세션 기반 인증)
- [x] 캐릭터 검색 SSE (DB-first → Kafka → INFO 수신 → SSE 응답)
- [x] 모험단명 검색 (DB only)

### 진행 예정

- [ ] 캐릭터 상세 페이지 (`/api/character/{serverId}/{characterId}`)
  - [ ] `character_detail` Kafka 발행 / `character_detail_ready` 수신
  - [ ] 능력치 / 장비 / 아바타 / 크리쳐 / 스킬 탭용 Entity 추가
  - [ ] InfoConsumer의 `handleDetailReady` 구현
- [ ] 장비 시뮬레이션
  - [ ] 장비 교체 후 스펙 변화량 계산 (SimService)
  - [ ] Redis 캐싱 연동
- [ ] 랭킹 (직업군별 명성/스펙 정렬)
- [ ] 타임라인 상세 조회
- [ ] 경매장 아이템 검색
- [ ] 마이페이지 (API 키 변경, 즐겨찾기)

---

## 빌드 및 실행

```bash
# 빌드 (테스트 스킵)
./gradlew build -x test

# 실행 (PostgreSQL, Redis, Kafka 기동 필요)
./gradlew bootRun
```

> 서버 포트: **8081**
> PostgreSQL: `127.0.0.1:5432/pgdb`
> Redis: `127.0.0.1:6379`
> Kafka: `127.0.0.1:9092`

---

## 개발 원칙

- **DB-first**: 캐릭터 조회는 항상 DB 먼저, 없을 때만 Comm 서버에 수집 요청
- **세션 기반 인증**: Spring Session + Redis, 세션 타임아웃 1800초
- **스키마 분리**: 회원 데이터(`users`)와 캐릭터 데이터(`character`)는 스키마로 분리