import { useState } from 'react';
import { useNavigate } from 'react-router-dom';
import useFavoriteStore from '../store/favoriteStore';
import useHistoryStore from '../store/historyStore';

const TABS = [
    { key: 'character', label: '캐릭터' },
    { key: 'auction',   label: '경매장' },
    { key: 'compare',   label: '캐릭터 비교' },
];

const CharacterTab = () => {
    const navigate = useNavigate();
    const favorites = useFavoriteStore((s) => s.favorites);
    const history = useHistoryStore((s) => s.history);

    return (
        <div style={styles.tabContent}>
            <section style={styles.section}>
                <h4 style={styles.sectionTitle}>⭐ 즐겨찾기</h4>
                {favorites.length === 0
                    ? <p style={styles.empty}>즐겨찾기한 캐릭터가 없습니다.<br/>캐릭터 상세 페이지에서 추가할 수 있어요.</p>
                    : <div style={styles.cardGrid}>
                        {favorites.map((c) => (
                            <div key={c.characterId} style={styles.charCard} onClick={() => navigate(`/char/${c.serverId}/${c.characterId}`)}>
                                <img src={`https://img-api.neople.co.kr/df/servers/${c.serverId}/characters/${c.characterId}?zoom=1`} alt={c.characterName} style={styles.cardImg} />
                                <div style={styles.cardName}>{c.characterName}</div>
                                <div style={styles.cardSub}>{c.serverName} · {c.jobName}</div>
                            </div>
                        ))}
                    </div>
                }
            </section>

            <div style={styles.divider} />

            <section style={styles.section}>
                <h4 style={styles.sectionTitle}>🕐 최근 본 캐릭터</h4>
                {history.length === 0
                    ? <p style={styles.empty}>최근 본 캐릭터가 없습니다.</p>
                    : <div style={styles.cardGrid}>
                        {history.map((c) => (
                            <div key={c.characterId} style={styles.charCard} onClick={() => navigate(`/char/${c.serverId}/${c.characterId}`)}>
                                <img src={`https://img-api.neople.co.kr/df/servers/${c.serverId}/characters/${c.characterId}?zoom=1`} alt={c.characterName} style={styles.cardImg} />
                                <div style={styles.cardName}>{c.characterName}</div>
                                <div style={styles.cardSub}>{c.serverName} · {c.jobName}</div>
                            </div>
                        ))}
                    </div>
                }
            </section>
        </div>
    );
};

const AuctionTab = () => (
    <div style={styles.tabContent}>
        <section style={styles.section}>
            <h4 style={styles.sectionTitle}>⭐ 즐겨찾기 아이템</h4>
            <p style={styles.empty}>즐겨찾기한 아이템이 없습니다. (준비 중)</p>
        </section>
        <div style={styles.divider} />
        <section style={styles.section}>
            <h4 style={styles.sectionTitle}>🕐 최근 검색 아이템</h4>
            <p style={styles.empty}>최근 검색한 아이템이 없습니다. (준비 중)</p>
        </section>
    </div>
);

const CompareTab = () => (
    <div style={styles.tabContent}>
        <section style={styles.section}>
            <h4 style={styles.sectionTitle}>캐릭터 비교</h4>
            <p style={styles.empty}>비교할 캐릭터를 선택하세요. (준비 중)</p>
        </section>
    </div>
);

const Dashboard = () => {
    const [activeTab, setActiveTab] = useState('character');

    const renderTab = () => {
        switch (activeTab) {
            case 'character': return <CharacterTab />;
            case 'auction':   return <AuctionTab />;
            case 'compare':   return <CompareTab />;
            default:          return null;
        }
    };

    return (
        <div style={styles.page}>
            <h2 style={styles.pageTitle}>대시보드</h2>

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

            <div style={styles.tabPanel}>
                {renderTab()}
            </div>
        </div>
    );
};

const styles = {
    page: { padding: '2rem 3rem' },
    pageTitle: { fontSize: '1.8rem', marginBottom: '1.5rem' },
    tabBar: { display: 'flex', gap: '4px', borderBottom: '2px solid #ddd' },
    tabBtn: { padding: '10px 24px', border: 'none', borderBottom: '2px solid transparent', background: 'transparent', cursor: 'pointer', fontSize: '0.95rem', color: '#666', marginBottom: '-2px', borderRadius: '0' },
    tabBtnActive: { color: '#2c3e50', fontWeight: 'bold', borderBottom: '2px solid #2c3e50' },
    tabPanel: { background: '#fff', border: '1px solid #ddd', borderTop: 'none', borderRadius: '0 0 8px 8px', minHeight: '400px' },
    tabContent: { padding: '1.5rem 2rem' },
    section: { marginBottom: '1rem' },
    sectionTitle: { fontSize: '1rem', fontWeight: 'bold', marginBottom: '1rem', color: '#333' },
    empty: { color: '#aaa', fontSize: '0.9rem', lineHeight: '1.6' },
    divider: { height: '1px', background: '#eee', margin: '1.5rem 0' },
    cardGrid: { display: 'grid', gridTemplateColumns: 'repeat(auto-fill, minmax(130px, 1fr))', gap: '16px' },
    charCard: { border: '1px solid #eee', borderRadius: '8px', padding: '12px', cursor: 'pointer', textAlign: 'center', transition: 'transform 0.15s', boxShadow: '0 1px 4px rgba(0,0,0,0.06)' },
    cardImg: { width: '80px', height: '80px', objectFit: 'contain' },
    cardName: { fontWeight: 'bold', fontSize: '0.9rem', marginTop: '8px' },
    cardSub: { fontSize: '0.75rem', color: '#888', marginTop: '2px' },
};

export default Dashboard;