import { Link } from 'react-router-dom';

const Navbar = () => {
    return (
        <nav style={styles.nav}>
            {/* 좌측 상단 로고 */}
            <div className="logo">
                <Link to="/" style={styles.logoLink}>
                    <span style={styles.logoText}>DF_SEARCH</span>
                </Link>
            </div>

            {/* 중앙 메뉴 (페이지 1, 2, 3 대신 실제 기능 할당) */}
            <ul style={styles.menuList}>
                <li><Link name="charSearch" to="/" style={styles.link}>캐릭터 검색</Link></li>
                <li><Link name="ranking" to="/ranking" style={styles.link}>랭킹</Link></li>
                <li><Link name="timeline" to="/timeline" style={styles.link}>타임라인</Link></li>
                <li><Link name="auction" to="/auction" style={styles.link}>경매장</Link></li>
                <li><Link name="raid" to="/raid" style={styles.link}>레이드 매니저</Link></li>
            </ul>

            {/* 우측 상단 로그인 */}
            <div className="auth">
                <Link to="/login" style={styles.loginBtn}>로그인</Link>
            </div>
        </nav>
    );
};

// 간단한 인라인 스타일 (나중에 CSS 파일로 분리하면 좋습니다)
const styles = {
    nav: { display: 'flex', justifyContent: 'space-between', padding: '1rem 2rem', background: '#2c3e50', color: '#fff' },
    logoLink: { textDecoration: 'none', color: '#fff' },
    logoText: { fontSize: '1.5rem', fontWeight: 'bold' },
    menuList: { display: 'flex', listStyle: 'none', gap: '20px', margin: 0 },
    link: { color: '#ecf0f1', textDecoration: 'none' },
    loginBtn: { color: '#fff', textDecoration: 'none', padding: '5px 10px', border: '1px solid #fff', borderRadius: '4px' }
};

export default Navbar;