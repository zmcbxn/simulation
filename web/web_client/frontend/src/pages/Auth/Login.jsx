import { useState } from 'react';
import { useNavigate, Link } from 'react-router-dom';
import apiClient from '../../api/apiClient';
import useAuthStore from '../../store/authStore';

const Login = () => {
    const navigate = useNavigate();
    const login = useAuthStore((state) => state.login);

    const [form, setForm] = useState({ username: '', password: '' });
    const [error, setError] = useState('');
    const [loading, setLoading] = useState(false);

    const handleChange = (e) => {
        setForm({ ...form, [e.target.name]: e.target.value });
    };

    const handleSubmit = async (e) => {
        e.preventDefault();
        setError('');
        setLoading(true);
        try {
            const res = await apiClient.post('/auth/login', form);
            login(res.data);
            navigate('/');
        } catch (err) {
            setError(err.response?.data?.message || '로그인에 실패했습니다.');
        } finally {
            setLoading(false);
        }
    };

    return (
        <div style={styles.wrapper}>
            <div style={styles.box}>
                <h2 style={styles.title}>로그인</h2>
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
                    {error && <div style={styles.error}>{error}</div>}
                    <button style={styles.button} type="submit" disabled={loading}>
                        {loading ? '로그인 중...' : '로그인'}
                    </button>
                </form>
                <div style={styles.footer}>
                    계정이 없으신가요? <Link to="/register">회원가입</Link>
                </div>
            </div>
        </div>
    );
};

const styles = {
    wrapper: { display: 'flex', justifyContent: 'center', alignItems: 'center', height: '100vh', background: '#f0f2f5' },
    box: { background: '#fff', padding: '40px', borderRadius: '8px', boxShadow: '0 2px 10px rgba(0,0,0,0.1)', width: '360px' },
    title: { textAlign: 'center', marginBottom: '24px' },
    form: { display: 'flex', flexDirection: 'column', gap: '12px' },
    input: { padding: '10px 14px', border: '1px solid #ddd', borderRadius: '4px', fontSize: '1rem' },
    error: { color: '#e74c3c', fontSize: '0.9rem', textAlign: 'center' },
    button: { padding: '10px', background: '#2c3e50', color: '#fff', border: 'none', borderRadius: '4px', fontSize: '1rem', cursor: 'pointer' },
    footer: { textAlign: 'center', marginTop: '16px', fontSize: '0.9rem' },
};

export default Login;