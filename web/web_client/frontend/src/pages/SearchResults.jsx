import React, { useEffect, useState } from 'react';
import { useParams, useNavigate } from 'react-router-dom';
import apiClient from '../api/apiClient';

/**
 * @typedef {Object} Character
 * @property {string} characterId
 * @property {string} characterName
 * @property {string} serverId
 * @property {string} [serverName]
 * @property {string} jobName
 * @property {number} level
 */

const SearchResults = () => {
    const { serverId, name } = useParams();
    const navigate = useNavigate();

    /** @type {[Character[], React.Dispatch<React.SetStateAction<Character[]>>]} */
    const [results, setResults] = useState([]);
    const [loading, setLoading] = useState(true);

    useEffect(() => {
        const fetchResults = async () => {
            setLoading(true);
            try {
                const endpoint = serverId === 'adventureName'
                    ? `/api/search/adventureName/${name}`
                    : `/api/search/${serverId}/${name}`;

                const response = await apiClient.get(endpoint);
                setResults(response.data);
            } catch (error) {
                console.error("검색 실패:", error);
            } finally {
                setLoading(false);
            }
        };

        void fetchResults();
    }, [serverId, name]);

    if (loading) return <div style={styles.message}>캐릭터 정보를 불러오는 중...</div>;
    if (results.length === 0) return <div style={styles.message}>검색 결과가 없습니다.</div>;

    return (
        <div style={styles.container}>
            <h2 style={{ marginBottom: '30px' }}>'{name}' 검색 결과 ({results.length})</h2>
            <div style={styles.grid}>
                {results.map((char) => (
                    <div
                        key={char.characterId}
                        style={styles.card}
                        onClick={() => navigate(`/char/${char.serverId}/${char.characterId}`)}
                    >
                        <img
                            src={`https://img-api.neople.co.kr/df/servers/${char.serverId}/characters/${char.characterId}?zoom=1`}
                            alt={char.characterName}
                            style={styles.image}
                        />
                        <div style={styles.info}>
                            <div style={styles.charName}>{char.characterName}</div>
                            <div style={styles.subInfo}>{char.jobName} | {char.level}Lv</div>
                            <div style={styles.serverTag}>{char.serverName || char.serverId}</div>
                        </div>
                    </div>
                ))}
            </div>
        </div>
    );
};

/** @type {Object.<string, React.CSSProperties>} */
const styles = {
    container: { padding: '40px', textAlign: 'center' },
    grid: {
        display: 'grid',
        gridTemplateColumns: 'repeat(auto-fill, minmax(150px, 1fr))',
        gap: '20px',
        marginTop: '30px'
    },
    card: {
        border: '1px solid #ddd',
        borderRadius: '8px',
        padding: '15px',
        cursor: 'pointer',
        transition: 'transform 0.2s',
        backgroundColor: '#fff',
        boxShadow: '0 2px 5px rgba(0,0,0,0.1)'
    },
    image: { width: '100px', height: '100px', objectFit: 'contain' },
    info: { marginTop: '10px' },
    charName: { fontWeight: 'bold', fontSize: '1.1rem', marginBottom: '5px' },
    subInfo: { fontSize: '0.85rem', color: '#666' },
    serverTag: {
        display: 'inline-block',
        marginTop: '8px',
        fontSize: '0.75rem',
        padding: '2px 6px',
        background: '#eee',
        borderRadius: '4px'
    },
    message: { textAlign: 'center', marginTop: '100px', fontSize: '1.2rem' }
};

export default SearchResults;