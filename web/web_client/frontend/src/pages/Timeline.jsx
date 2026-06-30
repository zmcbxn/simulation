import { useState } from 'react';
import { useNavigate } from 'react-router-dom';
import { SERVER_LIST as serverList } from '../constants/servers';

const Timeline = () => {
    const navigate = useNavigate();

    const [selectedServer, setSelectedServer] = useState('all');
    const [searchName, setSearchName] = useState('');
    const [error, setError] = useState('');

    const handleSearch = (e) => {
        e.preventDefault();
        if (!searchName.trim()) {
            setError('닉네임을 입력해주세요.');
            return;
        }
        setError('');
        navigate(`/timeline/search/${selectedServer}/${searchName.trim()}`);
    };

    return (
        <div className="home-wrapper">
            <h1 className="main-logo">타임라인</h1>
            <div className="home-search-area">
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
                        onChange={(e) => { setSearchName(e.target.value); setError(''); }}
                        placeholder={selectedServer === 'adventureName' ? '모험단명을 입력하세요' : '캐릭터명을 입력하세요'}
                    />
                    <button className="search-button" type="submit">검색</button>
                </form>
                {error && <p className="search-error">{error}</p>}
            </div>
        </div>
    );
};

export default Timeline;
