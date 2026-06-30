import { useState } from 'react';
import { useNavigate } from 'react-router-dom';
import apiClient from '../api/apiClient';
import useAuthStore from '../store/authStore';

const MyPage = () => {
    const navigate = useNavigate();
    const { user } = useAuthStore();

    // API 키 수정
    const [apiKeySection, setApiKeySection] = useState('idle'); // idle | verify | edit
    const [verifyPassword, setVerifyPassword] = useState('');
    const [verifyError, setVerifyError] = useState('');
    const [newApiKey, setNewApiKey] = useState('');
    const [apiKeyMsg, setApiKeyMsg] = useState('');

    // 비밀번호 변경
    const [pwForm, setPwForm] = useState({ currentPassword: '', newPassword: '', confirmPassword: '' });
    const [pwMsg, setPwMsg] = useState('');
    const [pwError, setPwError] = useState('');

    const handleVerifyPassword = async (e) => {
        e.preventDefault();
        setVerifyError('');
        try {
            await apiClient.post('/auth/verify-password', { password: verifyPassword });
            setApiKeySection('edit');
            setVerifyPassword('');
        } catch {
            setVerifyError('비밀번호가 올바르지 않습니다.');
        }
    };

    const handleApiKeyUpdate = async (e) => {
        e.preventDefault();
        setApiKeyMsg('');
        try {
            await apiClient.put('/auth/api-key', { apiKey: newApiKey });
            setApiKeyMsg('API 키가 업데이트되었습니다.');
            setApiKeySection('idle');
            setNewApiKey('');
        } catch {
            setApiKeyMsg('업데이트에 실패했습니다.');
        }
    };

    const handlePasswordChange = async (e) => {
        e.preventDefault();
        setPwError('');
        setPwMsg('');
        if (pwForm.newPassword !== pwForm.confirmPassword) {
            setPwError('새 비밀번호가 일치하지 않습니다.');
            return;
        }
        try {
            await apiClient.put('/auth/password', {
                currentPassword: pwForm.currentPassword,
                newPassword: pwForm.newPassword,
            });
            setPwMsg('비밀번호가 변경되었습니다.');
            setPwForm({ currentPassword: '', newPassword: '', confirmPassword: '' });
        } catch (err) {
            setPwError(err.response?.data?.message || '변경에 실패했습니다.');
        }
    };

    return (
        <div style={styles.page}>
            <h2 style={styles.pageTitle}>마이페이지</h2>

            {/* 계정 정보 */}
            <section style={styles.card}>
                <h3 style={styles.cardTitle}>계정 정보</h3>
                <div style={styles.row}>
                    <span style={styles.label}>아이디</span>
                    <span>{user?.username}</span>
                </div>
            </section>

            {/* API 키 관리 */}
            <section style={styles.card}>
                <h3 style={styles.cardTitle}>API 키 관리</h3>
                <div style={styles.row}>
                    <span style={styles.label}>현재 API 키</span>
                    <span style={styles.masked}>••••••••••••••••</span>
                </div>

                {apiKeySection === 'idle' && (
                    <button style={styles.btn} onClick={() => setApiKeySection('verify')}>수정</button>
                )}

                {apiKeySection === 'verify' && (
                    <form onSubmit={handleVerifyPassword} style={styles.form}>
                        <p style={styles.hint}>API 키 수정을 위해 비밀번호를 확인합니다.</p>
                        <input
                            style={styles.input}
                            type="password"
                            placeholder="현재 비밀번호"
                            value={verifyPassword}
                            onChange={(e) => setVerifyPassword(e.target.value)}
                            required
                        />
                        {verifyError && <div style={styles.error}>{verifyError}</div>}
                        <div style={styles.btnRow}>
                            <button style={styles.btn} type="submit">확인</button>
                            <button style={styles.btnGhost} type="button" onClick={() => { setApiKeySection('idle'); setVerifyError(''); }}>취소</button>
                        </div>
                    </form>
                )}

                {apiKeySection === 'edit' && (
                    <form onSubmit={handleApiKeyUpdate} style={styles.form}>
                        <input
                            style={styles.input}
                            type="text"
                            placeholder="새 Nexon API 키"
                            value={newApiKey}
                            onChange={(e) => setNewApiKey(e.target.value)}
                            required
                        />
                        {apiKeyMsg && <div style={apiKeyMsg.includes('실패') ? styles.error : styles.success}>{apiKeyMsg}</div>}
                        <div style={styles.btnRow}>
                            <button style={styles.btn} type="submit">저장</button>
                            <button style={styles.btnGhost} type="button" onClick={() => setApiKeySection('idle')}>취소</button>
                        </div>
                    </form>
                )}

                {apiKeySection === 'idle' && apiKeyMsg && (
                    <div style={styles.success}>{apiKeyMsg}</div>
                )}
            </section>

            {/* 비밀번호 변경 */}
            <section style={styles.card}>
                <h3 style={styles.cardTitle}>비밀번호 변경</h3>
                <form onSubmit={handlePasswordChange} style={styles.form}>
                    <input
                        style={styles.input}
                        type="password"
                        placeholder="현재 비밀번호"
                        value={pwForm.currentPassword}
                        onChange={(e) => setPwForm({ ...pwForm, currentPassword: e.target.value })}
                        required
                    />
                    <input
                        style={styles.input}
                        type="password"
                        placeholder="새 비밀번호"
                        value={pwForm.newPassword}
                        onChange={(e) => setPwForm({ ...pwForm, newPassword: e.target.value })}
                        required
                    />
                    <input
                        style={styles.input}
                        type="password"
                        placeholder="새 비밀번호 확인"
                        value={pwForm.confirmPassword}
                        onChange={(e) => setPwForm({ ...pwForm, confirmPassword: e.target.value })}
                        required
                    />
                    {pwError && <div style={styles.error}>{pwError}</div>}
                    {pwMsg && <div style={styles.success}>{pwMsg}</div>}
                    <button style={styles.btn} type="submit">변경</button>
                </form>
            </section>

        </div>
    );
};

const styles = {
    page: { maxWidth: '700px', margin: '0 auto', padding: '2rem 1rem' },
    pageTitle: { fontSize: '1.8rem', marginBottom: '1.5rem' },
    card: { background: '#fff', border: '1px solid #ddd', borderRadius: '10px', padding: '1.5rem', marginBottom: '1.2rem', boxShadow: '0 2px 6px rgba(0,0,0,0.05)' },
    cardTitle: { fontSize: '1.1rem', fontWeight: 'bold', marginBottom: '1rem', borderBottom: '1px solid #eee', paddingBottom: '0.5rem' },
    row: { display: 'flex', alignItems: 'center', gap: '1rem', marginBottom: '0.8rem' },
    label: { color: '#666', fontSize: '0.9rem', minWidth: '90px' },
    masked: { color: '#aaa', letterSpacing: '2px' },
    form: { display: 'flex', flexDirection: 'column', gap: '10px', marginTop: '0.8rem' },
    input: { padding: '9px 12px', border: '1px solid #ddd', borderRadius: '6px', fontSize: '0.95rem' },
    hint: { fontSize: '0.85rem', color: '#888', margin: 0 },
    btnRow: { display: 'flex', gap: '8px' },
    btn: { padding: '8px 20px', background: '#2c3e50', color: '#fff', border: 'none', borderRadius: '6px', cursor: 'pointer', fontSize: '0.9rem' },
    btnGhost: { padding: '8px 20px', background: 'transparent', color: '#555', border: '1px solid #ccc', borderRadius: '6px', cursor: 'pointer', fontSize: '0.9rem' },
    error: { color: '#e74c3c', fontSize: '0.85rem' },
    success: { color: '#27ae60', fontSize: '0.85rem' },
    empty: { color: '#aaa', fontSize: '0.9rem' },
};

export default MyPage;