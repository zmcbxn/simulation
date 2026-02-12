import React, { useState } from 'react';
import { useNavigate } from 'react-router-dom';

const Home = () => {
    const navigate = useNavigate();

    // 스크린샷 데이터 기반 서버 리스트
    const serverList = [
        { id: 'all', name: '전체 서버' },
        { id: 'adventureName', name: '모험단' },
        { id: 'cain', name: '카인' },
        { id: 'diregie', name: '디레지에' },
        { id: 'siroco', name: '시로코' },
        { id: 'prey', name: '프레이' },
        { id: 'casillas', name: '카시야스' },
        { id: 'hilder', name: '힐더' },
        { id: 'anton', name: '안톤' },
        { id: 'bakal', name: '바칼' },
    ];

    const [selectedServer, setSelectedServer] = useState('all');
    const [searchName, setSearchName] = useState('');

    const handleSearch = (e) => {
        e.preventDefault();
        if (!searchName.trim()) {
            alert('캐릭터명이나 모험단명을 입력해주세요!');
            return;
        }
        // 검색 결과 페이지(SearchResults)로 파라미터를 담아 이동합니다.
        navigate(`/search/${selectedServer}/${searchName}`);
    };

    return (
        <div className="home-search-wrapper">
            <div className="search-container">
                <h1 className="main-logo">DF_SEARCH</h1>
                <form className="search-form" onSubmit={handleSearch}>
                    <select
                        className="server-select"
                        value={selectedServer}
                        onChange={(e) => setSelectedServer(e.target.value)}
                    >
                        {serverList.map((server) => (
                            <option key={server.id} value={server.id}>
                                {server.name}
                            </option>
                        ))}
                    </select>

                    <input
                        className="search-input"
                        type="text"
                        value={searchName}
                        onChange={(e) => setSearchName(e.target.value)}
                        placeholder={selectedServer === 'adventureName' ? "모험단명을 입력하세요" : "캐릭터명을 입력하세요"}
                    />

                    <button className="search-button" type="submit">검색</button>
                </form>
            </div>
        </div>
    );
};

export default Home;