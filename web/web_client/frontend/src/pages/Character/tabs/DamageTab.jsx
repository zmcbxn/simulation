const DamageTab = ({ data }) => {
    return (
        <div style={styles.wrapper}>
            <p style={styles.placeholder}>딜표 (준비 중)</p>
        </div>
    );
};

const styles = {
    wrapper: { padding: '2rem' },
    placeholder: { color: '#888', textAlign: 'center', marginTop: '40px' },
};

export default DamageTab;