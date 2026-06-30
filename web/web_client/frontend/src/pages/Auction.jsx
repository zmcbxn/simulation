import { useState } from 'react';

const Auction = () => {
    const [searchName, setSearchName] = useState('');

    const handleSearch = (e) => {
        e.preventDefault();
    };

    return (
        <div style={styles.wrapper}>
            <h2 style={styles.title}>경매장</h2>
            <form style={styles.form} onSubmit={handleSearch}>
                <input
                    style={styles.input}
                    type="text"
                    value={searchName}
                    onChange={(e) => setSearchName(e.target.value)}
                    placeholder="아이템명을 입력하세요"
                />
                <button style={styles.button} type="submit">검색</button>
            </form>
            <p style={styles.desc}>경매장 아이템 검색 (준비 중)</p>
        </div>
    );
};

const styles = {
    wrapper: { padding: '3rem 2rem', textAlign: 'center' },
    title: { fontSize: '2rem', marginBottom: '2rem' },
    form: { display: 'flex', justifyContent: 'center', gap: '8px', marginBottom: '2rem' },
    input: { padding: '0.6rem 1rem', fontSize: '1rem', border: '1px solid #ccc', borderRadius: '6px', width: '300px' },
    button: { padding: '0.6rem 1.4rem', background: '#2c3e50', color: '#fff', border: 'none', borderRadius: '6px', cursor: 'pointer' },
    desc: { color: '#888' },
};

export default Auction;