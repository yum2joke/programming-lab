#pragma once

// 공격 엔티티 통합 규격
typedef enum {
    // --- 투사체 (Projectile) ---
    PROJECTILE_PLAYER_BULLET,
    PROJECTILE_BOSS_BULLET,
    PROJECTILE_PLASMA,
    PROJECTILE_TYPE_COUNT,

    // --- 광선 (Beam) ---
    BEAM_FIXED = PROJECTILE_TYPE_COUNT,
    BEAM_ROTATE,
    BEAM_PLAYER_TRACKING,
    ATTACK_TYPE_COUNT
} AttackEntityType;

// 투사체 시스템 호환용
typedef AttackEntityType ProjectileType;

// 발사 형태
typedef enum {
    ATTACK_SHAPE_SINGLE,    // 단발
    ATTACK_SHAPE_CIRCULAR,  // 원형 방사
    ATTACK_SHAPE_SPREAD     // 부채꼴 방사 (구현예정)
} AttackShapeType;

// 발사 전용 규격서
typedef struct {
    AttackShapeType shape;
    AttackEntityType attackType;
    
    union {
        struct { int count; } circular;
        //struct { int count; float spreadAngle; } spread;
    } shapeData;
} AttackDesc;