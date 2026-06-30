import { useState } from 'react';
import { useNavigate, Link } from 'react-router-dom';
import apiClient from '../../api/apiClient';

const Register = () => {
    const navigate = useNavigate();

    const [form, setForm] = useState({ username: '', password: '', apiKey: '' });
    const [error, setError] = useState('');
    const [loading, setLoading] = useState(false);
    const [success, setSuccess] = useState(false);

    const handleChange = (e) => {
        setForm({ ...form, [e.target.name]: e.target.value });
    };

    const handleSubmit = async (e) => {
        e.preventDefault();
        setError('');
        setLoading(true);
        try {
            await apiClient.post('/auth/register', form);
            setSuccess(true);
        } catch (err) {
            console.error('[Register] 회원가입 실패:', err.response?.status, err.response?.data ?? err.message);
            setError(err.response?.data?.message || '회원가입에 실패했습니다.');
        } finally {
            setLoading(false);
        }
    };

    return (
        <div style={styles.wrapper}>
            <div style={styles.box}>
                <h2 style={styles.title}>회원가입</h2>
                <form onSubmit={handleSubmit} style={styles.form}>
                    <input
                        style={styles.input}
                        type="text"
                        name="username"
                        placeholder="아이디"
                        value={form.username}
                        onChange={handleChange}
                        required
                    />
                    <input
                        style={styles.input}
                        type="password"
                        name="password"
                        placeholder="비밀번호"
                        value={form.password}
                        onChange={handleChange}
                        required
                    />
                    <input
                        style={styles.input}
                        type="text"
                        name="apiKey"
                        placeholder="Nexon API 키 (선택)"
                        value={form.apiKey}
                        onChange={handleChange}
                    />
                    <div style={styles.hint}>
                        API 키를 등록하면 개인 키로 요청 제한 없이 이용할 수 있습니다.
                    </div>
                    {error && <div style={styles.error}>{error}</div>}
                    <button style={styles.button} type="submit" disabled={loading}>
                        {loading ? '처리 중...' : '회원가입'}
                    </button>
                </form>
                <div style={styles.footer}>
                    이미 계정이 있으신가요? <Link to="/login">로그인</Link>
                </div>
            </div>

            {success && (
                <div style={styles.overlay}>
                    <div style={styles.modal}>
                        <div style={styles.modalIcon}>✓</div>
                        <h3 style={styles.modalTitle}>회원가입 완료!</h3>
                        <p style={styles.modalDesc}>가입이 성공적으로 완료되었습니다.</p>
                        <button style={styles.modalBtn} onClick={() => navigate('/login')}>
                            로그인하러 가기
                        </button>
                    </div>
                </div>
            )}
        </div>
    );
};

const styles = {
    wrapper: { display: 'flex', justifyContent: 'center', alignItems: 'center', height: '100vh', background: '#f0f2f5' },
    box: { background: '#fff', padding: '40px', borderRadius: '8px', boxShadow: '0 2px 10px rgba(0,0,0,0.1)', width: '360px' },
    title: { textAlign: 'center', marginBottom: '24px' },
    form: { display: 'flex', flexDirection: 'column', gap: '12px' },
    input: { padding: '10px 14px', border: '1px solid #ddd', borderRadius: '4px', fontSize: '1rem' },
    hint: { fontSize: '0.8rem', color: '#888' },
    error: { color: '#e74c3c', fontSize: '0.9rem', textAlign: 'center' },
    button: { padding: '10px', background: '#2c3e50', color: '#fff', border: 'none', borderRadius: '4px', fontSize: '1rem', cursor: 'pointer' },
    footer: { textAlign: 'center', marginTop: '16px', fontSize: '0.9rem' },
    overlay: {
        position: 'fixed', inset: 0,
        background: 'rgba(0,0,0,0.4)',
        display: 'flex', justifyContent: 'center', alignItems: 'center',
        zIndex: 1000,
    },
    modal: {
        background: '#fff', borderRadius: '12px', padding: '40px',
        textAlign: 'center', width: '300px',
        boxShadow: '0 4px 20px rgba(0,0,0,0.15)',
    },
    modalIcon: {
        width: '56px', height: '56px', borderRadius: '50%',
        background: '#2ecc71', color: '#fff',
        fontSize: '1.8rem', lineHeight: '56px',
        margin: '0 auto 16px',
    },
    modalTitle: { fontSize: '1.3rem', marginBottom: '8px' },
    modalDesc: { color: '#666', marginBottom: '24px', fontSize: '0.95rem' },
    modalBtn: {
        padding: '10px 24px', background: '#2c3e50', color: '#fff',
        border: 'none', borderRadius: '6px', fontSize: '1rem', cursor: 'pointer',
        width: '100%',
    },
};

export default Register;