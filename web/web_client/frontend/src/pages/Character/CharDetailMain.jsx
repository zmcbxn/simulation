import { useEffect, useState } from 'react';
import { useParams } from 'react-router-dom';
import apiClient from '../../api/apiClient';
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

// TODO: 백엔드 API 엔드포인트 확정 후 연결
// GET /api/character/:serverId/:characterId
// 응답: { base, status, equipment, avatar, creature, skill, buff, ... }
const fetchCharacterDetail = async (serverId, characterId) => {
    const res = await apiClient.get(`/api/character/${serverId}/${characterId}`);
    return res.data;
};

const MOCK = {
    characterId: '7deb7e1058bef8bfee9adb04e6cf7928',
    serverId: 'cain',
    serverName: '카인',
    characterName: '테스트캐릭터',
    jobName: '眞 웨펀마스터',
    level: 110,
    fame: 123456,
    adventureName: '테스트모험단',
};

const CharDetailMain = () => {
    const { serverId, characterId } = useParams();
    const [character, setCharacter] = useState(null);
    const [loading, setLoading] = useState(true);
    const [error, setError] = useState('');
    const [activeTab, setActiveTab] = useState('equipment');

    useEffect(() => {
        const load = async () => {
            setLoading(true);
            setError('');
            try {
                const data = await fetchCharacterDetail(serverId, characterId);
                setCharacter(data);
            } catch {
                // TODO: 백엔드 연동 후 목업 제거
                setCharacter(MOCK);
            } finally {
                setLoading(false);
            }
        };
        load();
    }, [serverId, characterId]);

    if (loading) return <div style={styles.stateBox}>캐릭터 정보를 불러오는 중...</div>;
    if (error)   return <div style={{ ...styles.stateBox, color: '#e74c3c' }}>{error}</div>;
    if (!character) return null;

    const imageUrl = `https://img-api.neople.co.kr/df/servers/${character.serverId}/characters/${character.characterId}?zoom=3`;

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
                        alt={character.characterName}
                        style={styles.charImage}
                        onError={(e) => { e.target.style.background = '#eee'; e.target.style.display = 'block'; }}
                    />
                </div>

                {/* 우측: 요약 정보 */}
                <div style={styles.summaryInfo}>
                    <div style={styles.charName}>{character.characterName}</div>
                    <div style={styles.charSub}>
                        {character.serverName} · {character.jobName} · {character.level}Lv
                    </div>
                    {character.adventureName && (
                        <div style={styles.adventure}>모험단: {character.adventureName}</div>
                    )}
                    {character.fame && (
                        <div style={styles.fame}>명성: {character.fame.toLocaleString()}</div>
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
    charName: { fontSize: '1.8rem', fontWeight: 'bold' },
    charSub: { fontSize: '1rem', color: '#555' },
    adventure: { fontSize: '0.9rem', color: '#777' },
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