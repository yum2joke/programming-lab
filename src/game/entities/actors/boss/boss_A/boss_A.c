#include "boss_A.h"
#include "config.h"

// 페이즈별 패턴 설정. 반드시 hpThreshold가 높은 순서대로 정렬
// patterns는 weight 내림차순 정렬 권장
static const BossPhase s_boss_a_phases[] = {
    {
        .hpThreshold = 1.0f,    // 100%~
        .patternCount = 2,
        .patterns = (const PhasePattern[]){
            { .type = PATTERN_ROTATING_SLOW_12, .duration = 1.0f, .weight = 0.5f },
            { .type = PATTERN_ROTATING_FAST_24, .duration = 1.0f, .weight = 0.5f }
        }
    },
    {
        .hpThreshold = 0.5f,    // 50%~
        .patternCount = 1,
        .patterns = (const PhasePattern[]){
            { .type = PATTERN_ROTATING_FAST_24, .duration = 8.0f, .weight = 1.0f }
        }
    }
};

// 전체 설계도
const BossDesc BossA_Desc = {
    .maxHp = BOSS_MAX_HP,
    .phaseCount = 2,
    .phases = s_boss_a_phases
};