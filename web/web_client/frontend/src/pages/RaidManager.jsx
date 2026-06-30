const RaidManager = () => {
    return (
        <div style={styles.wrapper}>
            <h2 style={styles.title}>레이드 매니저</h2>
            <p style={styles.desc}>파티 구성 및 레이드 일정 관리 (준비 중)</p>
        </div>
    );
};

const styles = {
    wrapper: { padding: '3rem 2rem', textAlign: 'center' },
    title: { fontSize: '2rem', marginBottom: '1rem' },
    desc: { color: '#888' },
};

export default RaidManager;