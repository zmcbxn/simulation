import { create } from 'zustand';

const useFavoriteStore = create((set) => ({
    favorites: [], // [{ characterId, serverId, characterName, jobName, level, serverName }]

    addFavorite: (char) => set((state) => {
        if (state.favorites.some((f) => f.characterId === char.characterId)) return state;
        return { favorites: [...state.favorites, char] };
    }),

    removeFavorite: (characterId) => set((state) => ({
        favorites: state.favorites.filter((f) => f.characterId !== characterId),
    })),
}));

export default useFavoriteStore;