import { useState, useRef, useEffect } from 'react';
import { Link, useNavigate } from 'react-router-dom';
import useAuthStore from '../../store/authStore';

const Navbar = () => {
    const navigate = useNavigate();
    const { isLoggedIn, user, logout } = useAuthStore();

    const [open, setOpen] = useState(false);
    const dropdownRef = useRef(null);

    useEffect(() => {
        const handleClickOutside = (e) => {
            if (dropdownRef.current && !dropdownRef.current.contains(e.target)) {
                setOpen(false);
            }
        };
        document.addEventListener('mousedown', handleClickOutside);
        return () => document.removeEventListener('mousedown', handleClickOutside);
    }, []);

    const handleLogout = () => {
        logout();
        navigate('/');
    };

    return (
        <nav style={styles.nav}>
            <div style={styles.left} />

            <ul style={styles.menuList}>
                <li><Link to="/" style={styles.link}>캐릭터 검색</Link></li>
                <li><Link to="/ranking" style={styles.link}>랭킹</Link></li>
                <li><Link to="/timeline" style={styles.link}>타임라인</Link></li>
                <li><Link to="/auction" style={styles.link}>경매장</Link></li>
                <li><Link to="/raid" style={styles.link}>레이드 매니저</Link></li>
                {isLoggedIn && <li><Link to="/dashboard" style={{ ...styles.link, color: '#f1c40f' }}>대시보드</Link></li>}
            </ul>

            <div style={styles.auth}>
                {isLoggedIn ? (
                    <div style={styles.dropdownWrapper} ref={dropdownRef}>
                        <button style={styles.userBtn} onClick={() => setOpen((v) => !v)}>
                            {user.username} ▾
                        </button>

                        {open && (
                            <div style={styles.dropdown}>
                                <div style={styles.section}>
                                    <div
                                        style={styles.item}
                                        onClick={() => { navigate('/mypage'); setOpen(false); }}
                                    >
                                        ⚙ 마이페이지
                                    </div>
                                    <div style={{ ...styles.item, color: '#e74c3c' }} onClick={handleLogout}>
                                        🚪 로그아웃
                                    </div>
                                </div>
                            </div>
                        )}
                    </div>
                ) : (
                    <Link to="/login" style={styles.loginBtn}>로그인</Link>
                )}
            </div>
        </nav>
    );
};

const styles = {
    nav: {
        display: 'grid',
        gridTemplateColumns: '1fr auto 1fr',
        alignItems: 'center',
        padding: '0.8rem 2rem',
        background: '#2c3e50',
        color: '#fff',
        width: '100%',
        boxSizing: 'border-box',
        position: 'relative',
        zIndex: 100,
    },
    left: { flex: 1 },
    menuList: { display: 'flex', listStyle: 'none', margin: 0, padding: 0 },
    link: { display: 'inline-block', color: '#ecf0f1', textDecoration: 'none', padding: '0.4rem 1rem', minWidth: '80px', textAlign: 'center' },
    auth: { display: 'flex', alignItems: 'center', justifyContent: 'flex-end' },
    loginBtn: { color: '#fff', textDecoration: 'none', padding: '5px 14px', border: '1px solid #fff', borderRadius: '4px' },
    dropdownWrapper: { position: 'relative' },
    userBtn: { color: '#fff', background: 'transparent', border: '1px solid #fff', borderRadius: '4px', padding: '5px 14px', cursor: 'pointer', fontSize: '0.95rem' },
    dropdown: {
        position: 'absolute', right: 0, top: 'calc(100% + 8px)',
        background: '#fff', color: '#333',
        borderRadius: '8px', boxShadow: '0 4px 16px rgba(0,0,0,0.15)',
        minWidth: '200px', overflow: 'hidden',
        zIndex: 200,
    },
    section: { padding: '8px 0' },
    sectionTitle: { padding: '4px 16px', fontSize: '0.75rem', color: '#999', fontWeight: 'bold', letterSpacing: '0.05em' },
    item: { padding: '8px 16px', cursor: 'pointer', fontSize: '0.9rem', display: 'flex', justifyContent: 'space-between', alignItems: 'center' },
    itemSub: { fontSize: '0.75rem', color: '#aaa' },
    empty: { padding: '4px 16px', fontSize: '0.85rem', color: '#bbb' },
    divider: { height: '1px', background: '#eee', margin: '0' },
};

export default Navbar;