const SLOT_ORDER = [
    'WEAPON', 'TITLE', 'JACKET', 'PANTS', 'SHOULDER',
    'WAIST', 'SHOES', 'AMULET', 'WRIST', 'RING',
    'SUPPORT', 'MAGIC_STON', 'EARRING',
];

const RARITY_COLORS = {
    '커먼':    { bg: '#9e9e9e', text: '#fff' },
    '언커먼':  { bg: '#4caf50', text: '#fff' },
    '레어':    { bg: '#834BE6', text: '#fff' },
    '유니크':  { bg: '#E200E1', text: '#fff' },
    '레전더리':{ bg: '#E17200', text: '#fff' },
    '에픽':    { bg: '#F6AF29', text: '#fff' },
    '태초':    { bg: '#68D6ED', text: '#222' },
    '신화':    { bg: '#ec4899', text: '#fff' },
    '크로니클':{ bg: '#06b6d4', text: '#fff' },
};

const RarityBadge = ({ rarity }) => {
    const color = RARITY_COLORS[rarity] ?? { bg: '#aaa', text: '#fff' };
    return (
        <div style={{ ...styles.rarityBadge, background: color.bg, color: color.text }}>
            {rarity}
        </div>
    );
};

const EquipmentRow = ({ item }) => (
    <div style={styles.row}>
        <RarityBadge rarity={item.itemRarity} />

        <span style={{ ...styles.reinforce, color: item.isAmplified ? '#ec4899' : '#222' }}>
            +{item.reinforceValue}
        </span>

        <div style={{ ...styles.itemImageBox, border: `4px solid ${(RARITY_COLORS[item.itemRarity] ?? { bg: '#aaa' }).bg}`, background: (RARITY_COLORS[item.itemRarity] ?? { bg: '#aaa' }).bg }}>
            <img
                src={`https://img-api.neople.co.kr/df/items/${item.itemId}`}
                alt={item.itemName}
                style={styles.itemImage}
                onError={(e) => { e.target.style.display = 'none'; }}
            />
        </div>

        <div style={styles.itemInfo}>
            <span style={styles.slotName}>{item.slotName}</span>
            <span style={styles.itemName}>{item.itemName}</span>
            {item.isAmplified && item.amplificationName && (
                <span style={styles.amplification}>{item.amplificationName}</span>
            )}
        </div>
    </div>
);

const EquipmentTab = ({ data }) => {
    const equipment = data?.equipment;

    if (!equipment || equipment.length === 0) {
        return <div style={styles.empty}>장착 장비 정보가 없습니다.</div>;
    }

    const sorted = [...equipment].sort(
        (a, b) => SLOT_ORDER.indexOf(a.slotId) - SLOT_ORDER.indexOf(b.slotId)
    );

    return (
        <div style={styles.wrapper}>
            {sorted.map((item) => (
                <EquipmentRow key={item.slotId} item={item} />
            ))}
        </div>
    );
};

const styles = {
    wrapper: { padding: '1rem 1.5rem', display: 'flex', flexDirection: 'column', gap: '6px' },
    empty: { padding: '2rem', textAlign: 'center', color: '#aaa' },
    row: {
        display: 'flex',
        alignItems: 'center',
        gap: '10px',
        padding: '10px 12px',
        border: '1px solid #eee',
        borderRadius: '8px',
        background: '#fafafa',
    },
    rarityBadge: {
        flexShrink: 0,
        fontSize: '0.7rem',
        fontWeight: 'bold',
        padding: '3px 6px',
        borderRadius: '4px',
        minWidth: '44px',
        textAlign: 'center',
    },
    reinforce: {
        flexShrink: 0,
        fontSize: '0.9rem',
        fontWeight: 'bold',
        minWidth: '30px',
        textAlign: 'center',
    },
    itemImageBox: {
        flexShrink: 0,
        width: '48px',
        height: '48px',
        background: '#aaa',
        borderRadius: '6px',
        overflow: 'hidden',
        display: 'flex',
        alignItems: 'center',
        justifyContent: 'center',
    },
    itemImage: { width: '48px', height: '48px', objectFit: 'contain' },
    itemInfo: {
        display: 'flex',
        flexDirection: 'column',
        gap: '2px',
        flex: 1,
        minWidth: 0,
    },
    slotName: {
        fontSize: '0.72rem',
        color: '#999',
    },
    itemName: {
        fontSize: '0.95rem',
        fontWeight: '500',
        color: '#222',
        overflow: 'hidden',
        textOverflow: 'ellipsis',
        whiteSpace: 'nowrap',
    },
    amplification: {
        fontSize: '0.75rem',
        color: '#ec4899',
    },
};

export default EquipmentTab;
