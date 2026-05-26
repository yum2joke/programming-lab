#pragma once

// 공격 엔티티 통합 규격
typedef enum {
    // --- 투사체 (Projectile) ---
    PROJECTILE_PLAYER_BULLET,
    PROJECTILE_BOSS_BULLET,
    PROJECTILE_PLASMA,
    PROJECTILE_TYPE_COUNT,

    // --- 광선 (Beam) ---
    // ...
    
} AttackEntityType;

// 투사체 시스템 호환용
typedef AttackEntityType ProjectileType;