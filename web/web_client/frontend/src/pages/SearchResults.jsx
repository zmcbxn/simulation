import { useEffect, useState } from 'react';
import { useParams, useNavigate } from 'react-router-dom';
import { SERVER_NAME_MAP } from '../constants/servers';

const SearchResults = () => {
    const { serverId, name } = useParams();
    const navigate = useNavigate();

    const [results, setResults] = useState([]);
    const [loading, setLoading] = useState(true);
    const [error, setError] = useState('');

    useEffect(() => {
        setLoading(true);
        setError('');
        setResults([]);

        const url = serverId === 'adventureName'
            ? `/api/search/adventureName/${encodeURIComponent(name)}`
            : `/api/search/sse/${serverId}/${encodeURIComponent(name)}`;

        if (serverId === 'adventureName') {
            fetch(url, { credentials: 'include' })
                .then(res => res.json())
                .then(data => { setResults(data); setLoading(false); })
                .catch(() => { setError('검색 중 오류가 발생했습니다.'); setLoading(false); });
            return;
        }

        const eventSource = new EventSource(url);

        eventSource.addEventListener('result', (e) => {
            setResults(JSON.parse(e.data));
            setLoading(false);
            eventSource.close();
        });

        eventSource.addEventListener('error', (e) => {
            setError(e.data ? JSON.parse(e.data).message : '검색 중 오류가 발생했습니다.');
            setLoading(false);
            eventSource.close();
        });

        eventSource.onerror = () => {
            setError('서버 연결에 실패했습니다.');
            setLoading(false);
            eventSource.close();
        };

        return () => eventSource.close();
    }, [serverId, name]);

    return (
        <div style={styles.container}>
            <h2 style={styles.heading}>
                '{name}' 검색 결과
                {!loading && !error && <span style={styles.count}> ({results.length})</span>}
            </h2>

            {loading && (
                <div style={styles.stateBox}>캐릭터 정보를 불러오는 중...</div>
            )}

            {!loading && error && (
                <div style={{ ...styles.stateBox, color: '#e74c3c' }}>{error}</div>
            )}

            {!loading && !error && results.length === 0 && (
                <div style={styles.stateBox}>검색 결과가 없습니다.</div>
            )}

            {!loading && !error && results.length > 0 && (
                <div style={styles.grid}>
                    {results.map((char) => (
                        <div
                            key={`${char.serverId}-${char.characterId}`}
                            style={styles.card}
                            onClick={() => navigate(`/char/${char.serverId}/${char.characterId}`)}
                            onMouseEnter={(e) => e.currentTarget.style.transform = 'translateY(-4px)'}
                            onMouseLeave={(e) => e.currentTarget.style.transform = 'translateY(0)'}
                        >
                            <img
                                src={`https://img-api.neople.co.kr/df/servers/${char.serverId}/characters/${char.characterId}?zoom=1`}
                                alt={char.characterName}
                                style={styles.image}
                            />
                            <div style={styles.info}>
                                <div style={styles.charName}>{char.characterName}</div>
                                <div style={styles.subInfo}>{char.jobName} | {char.level}Lv</div>
                                <div style={styles.serverTag}>{SERVER_NAME_MAP[char.serverId] ?? char.serverId}</div>
                            </div>
                        </div>
                    ))}
                </div>
            )}
        </div>
    );
};

const styles = {
    container: { padding: '2rem 3rem' },
    heading: { fontSize: '1.4rem', marginBottom: '1.5rem' },
    count: { color: '#888', fontSize: '1.1rem' },
    stateBox: { textAlign: 'center', marginTop: '80px', fontSize: '1.1rem', color: '#666' },
    grid: {
        display: 'grid',
        gridTemplateColumns: 'repeat(auto-fill, minmax(150px, 1fr))',
        gap: '20px',
    },
    card: {
        border: '1px solid #ddd',
        borderRadius: '10px',
        padding: '16px',
        cursor: 'pointer',
        transition: 'transform 0.2s, box-shadow 0.2s',
        backgroundColor: '#fff',
        boxShadow: '0 2px 6px rgba(0,0,0,0.08)',
        textAlign: 'center',
    },
    image: { width: '100px', height: '100px', objectFit: 'contain' },
    info: { marginTop: '10px' },
    charName: { fontWeight: 'bold', fontSize: '1rem', marginBottom: '4px' },
    subInfo: { fontSize: '0.82rem', color: '#666' },
    serverTag: {
        display: 'inline-block',
        marginTop: '8px',
        fontSize: '0.72rem',
        padding: '2px 8px',
        background: '#eee',
        borderRadius: '4px',
        color: '#555',
    },
};

export default SearchResults;
