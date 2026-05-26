#include "boss_A.h"
#include "config.h"

// 페이즈별 패턴 설정. 반드시 hpThreshold가 높은 순서대로 정렬
// patterns는 weight 내림차순 정렬 권장
static const BossPhase s_boss_a_phases[] = {
    {
        .hpThreshold = 1.0f,    // 100%~
        .patternCount = 3,
        .patterns = (const PhasePattern[]){
            { 
                .type = PATTERN_AIMED, .weight = 0.4f, 
                .desc = { .duration = 1.0f, .attackType = PROJECTILE_PLASMA, .count = 3, .interval = 0.3f }
            },
            { 
                .type = PATTERN_ROTATING, .weight = 0.3f, 
                .desc = { .duration = 1.0f, .attackType = PROJECTILE_BOSS_BULLET, .count = 12, .speed = BOSS_ROTATION_SPEED * 0.7f, .interval = BOSS_BURST_INTERVAL }
            },
            { 
                .type = PATTERN_ROTATING, .weight = 0.3f, 
                .desc = { .duration = 1.0f, .attackType = PROJECTILE_BOSS_BULLET, .count = 24, .speed = BOSS_ROTATION_SPEED * 1.2f, .interval = BOSS_BURST_INTERVAL * 0.8f }
            }
        }
    },
    {
        .hpThreshold = 0.5f,    // 50%~
        .patternCount = 2,
        .patterns = (const PhasePattern[]){
            { 
                .type = PATTERN_ROTATING, .weight = 0.7f, 
                .desc = { .duration = 5.0f, .attackType = PROJECTILE_BOSS_BULLET, .count = 24, .speed = BOSS_ROTATION_SPEED * 1.2f, .interval = BOSS_BURST_INTERVAL * 0.8f }
            },
            { 
                .type = PATTERN_AIMED, .weight = 0.3f, 
                .desc = { .duration = 1.0f, .attackType = PROJECTILE_PLASMA, .count = 3, .interval = 0.3f }
            }
        }
    }
};

// 전체 설계도
const BossDesc BossA_Desc = {
    .maxHp = BOSS_MAX_HP,
    .phaseCount = 2,
    .phases = s_boss_a_phases
};