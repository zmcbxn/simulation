import { useState, useRef } from 'react';
import { useNavigate } from 'react-router-dom';
import { SERVER_LIST as serverList, SERVER_NAME_MAP } from '../constants/servers';

const MAX_HISTORY = 10;

const getHistory = () => {
    try { return JSON.parse(localStorage.getItem('df_search_history')) || []; }
    catch { return []; }
};

const saveHistory = (serverId, query) => {
    const prev = getHistory();
    const filtered = prev.filter((h) => !(h.serverId === serverId && h.query === query));
    const serverName = SERVER_NAME_MAP[serverId] ?? (serverId === 'all' ? '전체 서버' : '모험단');
    const updated = [{ serverId, serverName, query }, ...filtered].slice(0, MAX_HISTORY);
    localStorage.setItem('df_search_history', JSON.stringify(updated));
};

const HistoryItem = ({ item, full, onClick, onDelete }) => {
    const [hovered, setHovered] = useState(false);
    return (
        <div
            style={{ ...(full ? styles.itemFull : styles.itemHalf), background: hovered ? '#f5f5f5' : 'transparent' }}
            onMouseEnter={() => setHovered(true)}
            onMouseLeave={() => setHovered(false)}
            onMouseDown={onClick}
        >
            <span style={styles.serverBadge}>{item.serverName}</span>
            <span style={styles.queryText}>{item.query}</span>
            <button style={styles.deleteBtn} onMouseDown={(e) => { e.stopPropagation(); onDelete(e); }}>✕</button>
        </div>
    );
};

const Home = () => {
    const navigate = useNavigate();
    const [selectedServer, setSelectedServer] = useState('all');
    const [searchName, setSearchName] = useState('');
    const [error, setError] = useState('');
    const [showHistory, setShowHistory] = useState(false);
    const [history, setHistory] = useState([]);
    const inputRef = useRef(null);

    const handleFocus = () => {
        setHistory(getHistory());
        setShowHistory(true);
    };

    const handleBlur = () => {
        setTimeout(() => setShowHistory(false), 150);
    };

    const doSearch = (serverId, query) => {
        setError('');
        saveHistory(serverId, query);
        setHistory(getHistory());
        navigate(`/search/${serverId}/${query.trim()}`);
    };

    const handleSearch = (e) => {
        e.preventDefault();
        if (!searchName.trim()) { setError('닉네임을 입력해주세요.'); return; }
        doSearch(selectedServer, searchName.trim());
    };

    const handleHistoryClick = (item) => {
        setSelectedServer(item.serverId);
        setSearchName(item.query);
        doSearch(item.serverId, item.query);
    };

    const deleteHistoryItem = (e, index) => {
        e.stopPropagation();
        const updated = history.filter((_, i) => i !== index);
        localStorage.setItem('df_search_history', JSON.stringify(updated));
        setHistory(updated);
    };

    return (
        <div className="home-wrapper">
            <h1 className="main-logo">DF_SEARCH</h1>
            <div className="home-search-area">
                <form className="search-form" onSubmit={handleSearch}>
                    <select
                        className="server-select"
                        value={selectedServer}
                        onChange={(e) => setSelectedServer(e.target.value)}
                    >
                        {serverList.map((server) => (
                            <option key={server.id} value={server.id}>{server.name}</option>
                        ))}
                    </select>
                    <div style={{ position: 'relative', flex: 1 }}>
                        <input
                            ref={inputRef}
                            className="search-input"
                            style={{ width: '100%', boxSizing: 'border-box' }}
                            type="text"
                            value={searchName}
                            onChange={(e) => { setSearchName(e.target.value); setError(''); }}
                            placeholder={selectedServer === 'adventureName' ? '모험단명을 입력하세요' : '캐릭터명을 입력하세요'}
                            onFocus={handleFocus}
                            onBlur={handleBlur}
                            autoComplete="off"
                        />
                        {showHistory && history.length > 0 && (
                            <div style={styles.dropdown}>
                                <div style={styles.grid}>
                                    {history.map((item, i) => (
                                        <HistoryItem
                                            key={i}
                                            item={item}
                                            full={i < 5}
                                            onClick={() => handleHistoryClick(item)}
                                            onDelete={(e) => deleteHistoryItem(e, i)}
                                        />
                                    ))}
                                </div>
                            </div>
                        )}
                    </div>
                    <button className="search-button" type="submit">검색</button>
                </form>
                {error && <p className="search-error">{error}</p>}
            </div>
        </div>
    );
};

const styles = {
    dropdown: {
        position: 'absolute',
        top: 'calc(100% + 4px)',
        left: 0,
        right: 0,
        background: '#fff',
        border: '1px solid #ddd',
        borderRadius: '8px',
        boxShadow: '0 4px 16px rgba(0,0,0,0.12)',
        zIndex: 200,
        padding: '8px',
    },
    grid: {
        display: 'grid',
        gridTemplateColumns: '1fr 1fr',
        gap: '4px',
    },
    itemFull: {
        gridColumn: '1 / -1',
        display: 'flex',
        alignItems: 'center',
        gap: '8px',
        padding: '7px 10px',
        borderRadius: '6px',
        cursor: 'pointer',
        fontSize: '0.9rem',
    },
    itemHalf: {
        display: 'flex',
        alignItems: 'center',
        gap: '8px',
        padding: '7px 10px',
        borderRadius: '6px',
        cursor: 'pointer',
        fontSize: '0.9rem',
    },
    serverBadge: {
        background: '#f0f0f0',
        color: '#888',
        fontSize: '0.75rem',
        padding: '2px 7px',
        borderRadius: '4px',
        whiteSpace: 'nowrap',
        flexShrink: 0,
    },
    queryText: {
        color: '#333',
        overflow: 'hidden',
        textOverflow: 'ellipsis',
        whiteSpace: 'nowrap',
        flex: 1,
    },
    deleteBtn: {
        marginLeft: 'auto',
        flexShrink: 0,
        background: 'none',
        border: 'none',
        color: '#bbb',
        cursor: 'pointer',
        fontSize: '0.75rem',
        padding: '0 2px',
        lineHeight: 1,
    },
};

export default Home;
