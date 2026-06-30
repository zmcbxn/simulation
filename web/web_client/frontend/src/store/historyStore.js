import { create } from 'zustand';

const MAX_HISTORY = 10;

const useHistoryStore = create((set) => ({
    history: [], // [{ characterId, serverId, characterName, jobName, level, serverName, viewedAt }]

    addHistory: (char) => set((state) => {
        const filtered = state.history.filter((h) => h.characterId !== char.characterId);
        const updated = [{ ...char, viewedAt: Date.now() }, ...filtered];
        return { history: updated.slice(0, MAX_HISTORY) };
    }),

    clearHistory: () => set({ history: [] }),
}));

export default useHistoryStore;