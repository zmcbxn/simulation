export const SERVER_LIST = [
    { id: 'all',           name: '전체 서버' },
    { id: 'adventureName', name: '모험단' },
    { id: 'cain',          name: '카인' },
    { id: 'diregie',       name: '디레지에' },
    { id: 'siroco',        name: '시로코' },
    { id: 'prey',          name: '프레이' },
    { id: 'casillas',      name: '카시야스' },
    { id: 'hilder',        name: '힐더' },
    { id: 'anton',         name: '안톤' },
    { id: 'bakal',         name: '바칼' },
];

export const SERVER_NAME_MAP = Object.fromEntries(
    SERVER_LIST.map((s) => [s.id, s.name])
);