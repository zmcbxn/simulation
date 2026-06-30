const SkillTreeTab = ({ data }) => {
    return (
        <div style={styles.wrapper}>
            <p style={styles.placeholder}>스킬 트리 (준비 중)</p>
        </div>
    );
};

const styles = {
    wrapper: { padding: '2rem' },
    placeholder: { color: '#888', textAlign: 'center', marginTop: '40px' },
};

export default SkillTreeTab;