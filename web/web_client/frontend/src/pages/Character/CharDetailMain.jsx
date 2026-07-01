import { useEffect, useState, useCallback } from 'react';
import { useParams, useNavigate } from 'react-router-dom';
import { SERVER_NAME_MAP } from '../../constants/servers';
import EquipmentTab from './tabs/EquipmentTab';
import InfoTab from './tabs/InfoTab';
import AvatarTab from './tabs/AvatarTab';
import BuffTab from './tabs/BuffTab';
import SkillTab from './tabs/SkillTab';
import DamageTab from './tabs/DamageTab';
import SkillTreeTab from './tabs/SkillTreeTab';

const TABS = [
    { key: 'equipment', label: '장착장비' },
    { key: 'info',      label: '세부스탯' },
    { key: 'avatar',    label: '아바타&크리쳐' },
    { key: 'buff',      label: '버프강화' },
    { key: 'skill',     label: '스킬개화' },
    { key: 'damage',    label: '딜표' },
    { key: 'skilltree', label: '스킬트리' },
];


const CharDetailMain = () => {
    const { serverId, characterId } = useParams();
    const navigate = useNavigate();
    const [character, setCharacter] = useState(null);
    const [loading, setLoading] = useState(true);
    const [error, setError] = useState('');
    const [activeTab, setActiveTab] = useState('equipment');
    const [refreshing, setRefreshing] = useState(false);
    const [fetchTrigger, setFetchTrigger] = useState(0);

    const handleRefresh = useCallback(() => {
        if (refreshing) return;
        setRefreshing(true);

        const es = new EventSource(`/api/character/${serverId}/${characterId}/refresh`);

        es.addEventListener('result', (e) => {
            const { refreshed, cooldownSeconds } = JSON.parse(e.data);
            setRefreshing(false);
            es.close();
            if (refreshed) {
                setFetchTrigger((n) => n + 1);
            } else {
                alert(`${cooldownSeconds}초 후 새로고침 가능합니다.`);
            }
        });

        es.onerror = () => {
            setRefreshing(false);
            alert('새로고침 요청에 실패했습니다.');
            es.close();
        };
    }, [serverId, characterId, refreshing]);

    useEffect(() => {
        setLoading(true);
        setError('');
        setCharacter(null);

        const es = new EventSource(`/api/character/${serverId}/${characterId}`);

        es.addEventListener('result', (e) => {
            setCharacter(JSON.parse(e.data));
            setLoading(false);
            es.close();
        });

        es.addEventListener('error', (e) => {
            setError(e.data ? JSON.parse(e.data).message : '캐릭터 정보를 불러오지 못했습니다.');
            setLoading(false);
            es.close();
        });

        es.onerror = () => {
            setError('서버 연결에 실패했습니다.');
            setLoading(false);
            es.close();
        };

        return () => es.close();
    }, [serverId, characterId, fetchTrigger]);

    if (loading) return <div style={styles.stateBox}>캐릭터 정보를 불러오는 중...</div>;
    if (error)   return <div style={{ ...styles.stateBox, color: '#e74c3c' }}>{error}</div>;
    if (!character) return null;

    const st = character.status;
    const serverName = SERVER_NAME_MAP[st.serverId] ?? st.serverId;
    const imageUrl = `https://img-api.neople.co.kr/df/servers/${st.serverId}/characters/${st.characterId}?zoom=3`;

    const renderTab = () => {
        switch (activeTab) {
            case 'equipment': return <EquipmentTab data={character} />;
            case 'info':      return <InfoTab data={character} />;
            case 'avatar':    return <AvatarTab data={character} />;
            case 'buff':      return <BuffTab data={character} />;
            case 'skill':     return <SkillTab data={character} />;
            case 'damage':    return <DamageTab data={character} />;
            case 'skilltree': return <SkillTreeTab data={character} />;
            default:          return null;
        }
    };

    return (
        <div style={styles.page}>
            {/* 상단 요약 패널 */}
            <div style={styles.summary}>
                {/* 좌측: 캐릭터 이미지 */}
                <div style={styles.imageBox}>
                    <img
                        src={imageUrl}
                        alt={st.characterName}
                        style={styles.charImage}
                        onError={(e) => { e.target.style.background = '#eee'; e.target.style.display = 'block'; }}
                    />
                </div>

                {/* 우측: 요약 정보 */}
                <div style={styles.summaryInfo}>
                    <div style={styles.summaryHeader}>
                        <div style={styles.charName}>{st.characterName}</div>
                        <button
                            style={styles.refreshBtn}
                            onClick={handleRefresh}
                            title="새로고침"
                            disabled={refreshing}
                        >
                            <svg
                                className={refreshing ? 'spinning' : ''}
                                width="18" height="18" viewBox="0 0 24 24"
                                fill="none" stroke="currentColor" strokeWidth="2.5"
                                strokeLinecap="round" strokeLinejoin="round"
                            >
                                <path d="M23 4v6h-6"/>
                                <path d="M20.49 15a9 9 0 1 1-2.12-9.36L23 10"/>
                            </svg>
                        </button>
                    </div>
                    {st.adventureName && (
                        <div
                            style={{ ...styles.charSub, ...styles.adventureLink }}
                            onClick={() => navigate(`/search/adventureName/${st.adventureName}`)}
                        >
                            {st.adventureName}
                        </div>
                    )}
                    <div style={styles.charSub}>
                        {serverName} · {st.jobName} · Lv.{st.level}
                    </div>
                    {st.guildName && (
                        <div style={styles.meta}>길드: {st.guildName}</div>
                    )}
                    {st.fame != null && (
                        <div style={styles.fame}>명성: {st.fame.toLocaleString()}</div>
                    )}
                </div>
            </div>

            {/* 탭 네비게이션 */}
            <div style={styles.tabBar}>
                {TABS.map((tab) => (
                    <button
                        key={tab.key}
                        style={activeTab === tab.key ? { ...styles.tabBtn, ...styles.tabBtnActive } : styles.tabBtn}
                        onClick={() => setActiveTab(tab.key)}
                    >
                        {tab.label}
                    </button>
                ))}
            </div>

            {/* 탭 내용 */}
            <div style={styles.tabContent}>
                {renderTab()}
            </div>
        </div>
    );
};

const styles = {
    page: { padding: '2rem 3rem' },
    stateBox: { textAlign: 'center', marginTop: '100px', fontSize: '1.1rem', color: '#666' },
    summary: {
        display: 'flex',
        alignItems: 'stretch',
        gap: '0',
        background: '#fff',
        border: '1px solid #ddd',
        borderRadius: '12px',
        marginBottom: '1.5rem',
        boxShadow: '0 2px 6px rgba(0,0,0,0.06)',
        overflow: 'hidden',
    },
    imageBox: {
        width: '200px',
        flexShrink: 0,
        background: '#f5f5f5',
        display: 'flex',
        alignItems: 'center',
        justifyContent: 'center',
        borderRight: '1px solid #ddd',
    },
    charImage: { width: '180px', height: '180px', objectFit: 'contain' },
    summaryInfo: { display: 'flex', flexDirection: 'column', gap: '10px', padding: '1.5rem 2rem', flex: 1 },
    summaryHeader: { display: 'flex', alignItems: 'center', gap: '10px' },
    charName: { fontSize: '1.8rem', fontWeight: 'bold' },
    refreshBtn: {
        background: 'none',
        border: '1px solid #ddd',
        borderRadius: '6px',
        padding: '6px 8px',
        cursor: 'pointer',
        color: '#555',
        display: 'flex',
        alignItems: 'center',
        justifyContent: 'center',
        flexShrink: 0,
    },
    charSub: { fontSize: '1rem', color: '#555' },
    adventureLink: { cursor: 'pointer', color: '#2980b9' },
    meta: { fontSize: '0.9rem', color: '#777' },
    fame: { fontSize: '0.95rem', color: '#2c3e50', fontWeight: '500' },
    tabBar: {
        display: 'flex',
        gap: '4px',
        borderBottom: '2px solid #ddd',
        marginBottom: '0',
    },
    tabBtn: {
        padding: '10px 20px',
        border: 'none',
        borderBottom: '2px solid transparent',
        background: 'transparent',
        cursor: 'pointer',
        fontSize: '0.95rem',
        color: '#666',
        marginBottom: '-2px',
        borderRadius: '0',
    },
    tabBtnActive: {
        color: '#2c3e50',
        fontWeight: 'bold',
        borderBottom: '2px solid #2c3e50',
    },
    tabContent: {
        background: '#fff',
        border: '1px solid #ddd',
        borderTop: 'none',
        borderRadius: '0 0 8px 8px',
        minHeight: '300px',
    },
};

export default CharDetailMain;