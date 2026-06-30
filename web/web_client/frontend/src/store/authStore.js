import { create } from 'zustand';

const useAuthStore = create((set) => ({
    user: null,        // { username, apiKey } 또는 null
    isLoggedIn: false,

    login: (userData) => set({ user: userData, isLoggedIn: true }),
    logout: () => set({ user: null, isLoggedIn: false }),
}));

export default useAuthStore;