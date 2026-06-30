const Ranking = () => {
    return (
        <div style={styles.wrapper}>
            <h2 style={styles.title}>랭킹</h2>
            <p style={styles.desc}>직업군별 스펙 랭킹 (준비 중)</p>
        </div>
    );
};

const styles = {
    wrapper: { padding: '3rem 2rem', textAlign: 'center' },
    title: { fontSize: '2rem', marginBottom: '1rem' },
    desc: { color: '#888' },
};

export default Ranking;