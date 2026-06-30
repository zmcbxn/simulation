# cal_site / web_client (프론트엔드)

DNF(던전앤파이터) 캐릭터 정보 조회 및 스펙 시뮬레이션 서비스의 **프론트엔드**.
React + Vite 기반, 상태관리는 Zustand 사용.

---

## 기술 스택

| 항목 | 기술 |
|------|------|
| 프레임워크 | React 19 + Vite |
| 라우팅 | react-router-dom v7 |
| 상태관리 | Zustand |
| HTTP 클라이언트 | Axios |

---

## 페이지 구조

### 인증
| 페이지 | 경로 | 설명 |
|--------|------|------|
| 로그인 | `/login` | 아이디/비밀번호 로그인 |
| 회원가입 | `/register` | 아이디/비밀번호/Nexon API 키(선택) 등록, 완료 시 모달 표시 |

> 개인정보 없이 아이디+비밀번호+API 키만 사용.
> API 키 등록 시 개인 키로 요청 → 비로그인(공용 키) 대비 속도 제한 없음.

### 캐릭터
| 페이지 | 경로 | 설명 |
|--------|------|------|
| 검색 홈 | `/` | 서버 선택 + 캐릭터명/모험단명 검색 |
| 검색 결과 | `/search/:serverId/:name` | 검색 결과 카드 목록 |
| 캐릭터 상세 | `/char/:serverId/:characterId` | 장착장비/세부스탯/아바타&크리쳐/버프강화/스킬개화/딜표/스킬트리 탭 구성 |
| 장비 시뮬레이션 | `/char/:serverId/:characterId/simulation` | 장비 교체 후 변경된 스펙 시뮬레이션 |
| 커스텀 계산기 | `/char/:serverId/:characterId/calculate` | 사용자 정의 수식 계산기 |

### 대시보드 (로그인 전용)
| 페이지 | 경로 | 설명 |
|--------|------|------|
| 대시보드 | `/dashboard` | 캐릭터(즐겨찾기+최근검색) / 경매장(즐겨찾기+최근검색) / 캐릭터 비교 탭 |

### 마이페이지 (로그인 전용)
| 페이지 | 경로 | 설명 |
|--------|------|------|
| 마이페이지 | `/mypage` | 계정 정보 확인, API 키 수정(비밀번호 재확인), 비밀번호 변경 |

### 랭킹
| 페이지 | 경로 | 설명 |
|--------|------|------|
| 랭킹 | `/ranking` | 직업군별 스펙 랭킹 |

### 경매장
| 페이지 | 경로 | 설명 |
|--------|------|------|
| 경매장 홈 | `/auction` | 아이템 검색 |
| 경매장 결과 | `/auction/search/:itemName` | 검색된 아이템 목록 및 시세 |

### 타임라인
| 페이지 | 경로 | 설명 |
|--------|------|------|
| 타임라인 검색 | `/timeline` | 서버 선택 + 캐릭터명/모험단명 검색 |
| 타임라인 결과(캐릭터 선택) | `/timeline/search/:serverId/:name` | 검색된 캐릭터 카드 목록 |
| 타임라인 상세 | `/timeline/:serverId/:characterId` | 선택한 캐릭터의 활동 이벤트 목록 |

### 레이드 매니저 (예정)
| 페이지 | 경로 | 설명 |
|--------|------|------|
| 레이드 매니저 | `/raid` | 파티 구성 / 레이드 일정 관리 |

---

## 추가 검토 기능

- **공유 기능** - 캐릭터 정보 URL 공유 (비로그인도 접근 가능)

---

## 백엔드 통신 방식

| 구간 | 방식 | 비고 |
|------|------|------|
| 프론트 ↔ 백엔드(Logic 서버) | HTTP REST API | Axios 사용, baseURL: `/api`, 포트 8081 |
| 캐릭터 상세 조회 진행률 | SSE (Server-Sent Events) | Nexon API 11종 수집 중 진행률 표시용. 백엔드 연동 시 구현 |

> 현재는 로딩 스피너로 대체. SSE는 백엔드 연동 시 교체 예정.

---

## 디렉터리 구조

```
src/
├── api/
│   └── apiClient.js           # Axios 인스턴스 (baseURL: /api, withCredentials)
├── constants/
│   └── servers.js             # DNF 서버 목록 상수 (SERVER_LIST, SERVER_NAME_MAP)
├── store/
│   ├── authStore.js           # Zustand - 로그인 상태/유저 정보
│   ├── favoriteStore.js       # Zustand - 즐겨찾기 캐릭터 목록
│   └── historyStore.js        # Zustand - 최근 본 캐릭터 (최대 10개)
├── layouts/
│   └── MainLayout.jsx         # Navbar + Outlet 공통 레이아웃
├── components/
│   └── common/
│       ├── Navbar.jsx         # 메뉴 중앙, 로그인 우측 드롭다운(마이페이지/로그아웃)
│       │                      # 로그인 시 대시보드 메뉴 추가 표시
│       └── Footer.jsx
└── pages/
    ├── Home.jsx               # 캐릭터 검색 홈
    ├── SearchResults.jsx      # 캐릭터 검색 결과 (카드 그리드)
    ├── Dashboard.jsx          # 대시보드 (캐릭터/경매장/비교 탭)
    ├── MyPage.jsx             # 마이페이지 (API 키, 비밀번호)
    ├── Ranking.jsx            # 랭킹 (껍데기)
    ├── Auction.jsx            # 경매장 검색 홈 (껍데기)
    ├── Timeline.jsx           # 타임라인 검색 홈
    ├── TimelineResults.jsx    # 타임라인 캐릭터 선택 (카드 그리드)
    ├── RaidManager.jsx        # 레이드 매니저 (껍데기)
    ├── Auth/
    │   ├── Login.jsx
    │   └── Register.jsx       # 가입 완료 시 모달 표시
    └── Character/
        ├── CharDetailMain.jsx # 캐릭터 상세 (7탭 구성)
        ├── CustomCalc.jsx
        └── tabs/
            ├── EquipmentTab.jsx
            ├── InfoTab.jsx
            ├── AvatarTab.jsx
            ├── BuffTab.jsx
            ├── SkillTab.jsx
            ├── DamageTab.jsx
            └── SkillTreeTab.jsx
```

---

## Zustand 스토어

| 스토어 | 파일 | 관리 상태 |
|--------|------|----------|
| authStore | `store/authStore.js` | `user`, `isLoggedIn`, `login()`, `logout()` |
| favoriteStore | `store/favoriteStore.js` | `favorites[]`, `addFavorite()`, `removeFavorite()` |
| historyStore | `store/historyStore.js` | `history[]` (최대 10개), `addHistory()`, `clearHistory()` |

> 현재 store는 메모리 기반 (새로고침 시 초기화). 백엔드 연동 후 DB 동기화 예정.

---

## 진행 현황

### 완료
- [x] Vite + React 프로젝트 세팅
- [x] react-router-dom 라우팅 구조 (`App.jsx`)
- [x] 공통 레이아웃 (`MainLayout`, `Navbar`, `Footer`)
- [x] Navbar - 메뉴 중앙 정렬, 로그인 시 드롭다운 (마이페이지/로그아웃)
- [x] Navbar - 로그인 시 대시보드 메뉴 표시
- [x] 검색 홈 (`Home.jsx`) - 서버 선택 + 검색, 빈 입력 인라인 에러
- [x] 검색 결과 (`SearchResults.jsx`) - 카드 그리드, 로딩/에러/결과없음 상태 처리, 서버명 한글 표시, SSE 방식 적용
- [x] 서버 목록 상수화 (`constants/servers.js`) - `SERVER_LIST`, `SERVER_NAME_MAP` 공통 관리
- [x] Zustand - `authStore`, `favoriteStore`, `historyStore` 구성
- [x] 로그인 페이지 (`Login.jsx`) - 세션 자동 복원 (`/auth/me`)
- [x] 회원가입 페이지 (`Register.jsx`) - API 키 입력, 완료 모달
- [x] 캐릭터 상세 페이지 (`CharDetailMain.jsx`) - 7탭 껍데기, 요약 패널(이미지+정보)
- [x] 대시보드 (`Dashboard.jsx`) - 캐릭터/경매장/비교 탭 껍데기
- [x] 마이페이지 (`MyPage.jsx`) - API 키 수정(비밀번호 재확인), 비밀번호 변경
- [x] 타임라인 검색/결과 페이지
- [x] 랭킹/경매장/레이드 껍데기 페이지
- [x] vite.config.js proxy 수정 (포트 8081, changeOrigin 위치 수정)

### 진행 예정
- [ ] 캐릭터 상세 각 탭 구현 (장착장비/세부스탯/아바타&크리쳐/버프강화/스킬개화/딜표/스킬트리)
- [ ] 캐릭터 상세에서 즐겨찾기 추가/제거 버튼
- [ ] 캐릭터 상세 조회 로딩 진행률 (SSE 기반 % 표시, 백엔드 연동 시)
- [ ] 장비 시뮬레이션 페이지
- [ ] 커스텀 계산기 페이지
- [ ] 랭킹 페이지 구현
- [ ] 경매장 결과 페이지 + 라우트
- [ ] 타임라인 상세 페이지
- [ ] 대시보드 캐릭터 비교 탭 구현
- [ ] 경매장 즐겨찾기/최근검색 store 구현
- [ ] store 백엔드 DB 동기화 (로그인 시 즐겨찾기/히스토리 불러오기)
- [ ] 비로그인 공용 API 키 소진 시 에러 안내
- [ ] 백엔드 API 전체 연동
