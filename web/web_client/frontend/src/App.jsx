import { useEffect } from 'react';
import { BrowserRouter as Router, Routes, Route } from 'react-router-dom';
import MainLayout from './layouts/MainLayout';
import Home from './pages/Home';
import SearchResults from './pages/SearchResults';
import Login from './pages/Auth/Login';
import Register from './pages/Auth/Register';
import CharDetailMain from './pages/Character/CharDetailMain';
import Ranking from './pages/Ranking';
import Timeline from './pages/Timeline';
import TimelineResults from './pages/TimelineResults';
import Auction from './pages/Auction';
import RaidManager from './pages/RaidManager';
import MyPage from './pages/MyPage';
import Dashboard from './pages/Dashboard';
import apiClient from './api/apiClient';
import useAuthStore from './store/authStore';

import './App.css'

function App() {
    const login = useAuthStore((state) => state.login);

    useEffect(() => {
        apiClient.get('/auth/me')
            .then(res => login(res.data))
            .catch(() => {});
    }, []);

    return (
        <Router>
            <Routes>
                <Route path="/" element={<MainLayout />}>
                    {/* 1. 메인 검색창 */}
                    <Route index element={<Home />} />

                    {/* 2. 검색 결과 목록 (아이콘으로 표시되는 페이지) */}
                    <Route path="search/:serverId/:name" element={<SearchResults />} />

                    {/* 3. 캐릭터 상세 + 하위 기능 */}
                    <Route path="char/:serverId/:characterId">
                        <Route index element={<CharDetailMain />} />
                        <Route path="simulation" element={<div>장비 변경 시뮬레이션 페이지</div>} />
                        <Route path="calculate" element={<div>커스텀 계산기 페이지</div>} />
                    </Route>

                    {/* 기타 메뉴 */}
                    <Route path="ranking" element={<Ranking />} />
                    <Route path="timeline" element={<Timeline />} />
                    <Route path="timeline/search/:serverId/:name" element={<TimelineResults />} />
                    <Route path="timeline/:serverId/:characterId" element={<div>타임라인 상세 (준비 중)</div>} />
                    <Route path="auction" element={<Auction />} />
                    <Route path="raid" element={<RaidManager />} />
                    <Route path="mypage" element={<MyPage />} />
                    <Route path="dashboard" element={<Dashboard />} />
                </Route>

                <Route path="login" element={<Login />} />
                <Route path="register" element={<Register />} />
            </Routes>
        </Router>
    )
}

export default App