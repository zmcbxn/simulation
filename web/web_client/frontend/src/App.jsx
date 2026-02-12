import { BrowserRouter as Router, Routes, Route } from 'react-router-dom';
import MainLayout from './layouts/MainLayout';
import Home from './pages/Home';
import SearchResults from './pages/SearchResults';
// 새로 추가될 페이지들 (껍데기 파일이 있다고 가정)
// import SearchResults from './pages/SearchResults';
// import CharDetailMain from './pages/Character/CharDetailMain';
// import SimulationPage from './pages/Character/SimulationPage';

import './App.css'

function App() {
    return (
        <Router>
            <Routes>
                <Route path="/" element={<MainLayout />}>
                    {/* 1. 메인 검색창 */}
                    <Route index element={<Home />} />

                    {/* 2. 검색 결과 목록 (아이콘으로 표시되는 페이지) */}
                    <Route path="search/:serverId/:name" element={<SearchResults />} />

                    {/* 3. 캐릭터 상세 정보 (내부 탭은 State로 관리하므로 중첩 Route 제거) */}
                    <Route path="char/:serverId/:name" element={<div>캐릭터 상세 정보 (기본/스킬 탭 포함)</div>} />

                    {/* 4. 장비 변경 (새로운 페이지로 이동) */}
                    <Route path="simulation/:serverId/:name" element={<div>장비 변경 시뮬레이션 페이지</div>} />

                    {/* 5. 계산기 (새로운 페이지로 이동) */}
                    <Route path="calculate/:serverId/:name" element={<div>커스텀 계산기 페이지</div>} />

                    {/* 기타 메뉴 */}
                    <Route path="ranking" element={<div>랭킹 검색</div>} />
                    <Route path="timeline" element={<div>타임라인</div>} />
                    <Route path="auction" element={<div>경매장</div>} />
                    <Route path="raid" element={<div>레이드</div>} />
                </Route>

                <Route path="login" element={<div>로그인</div>} />
                <Route path="register" element={<div>회원가입</div>} />
            </Routes>
        </Router>
    )
}

export default App