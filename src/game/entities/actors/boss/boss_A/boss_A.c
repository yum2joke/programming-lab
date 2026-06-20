#include "boss_A.h"

#include "config.h"

// 페이즈별 패턴 설정. 반드시 hpThreshold가 높은 순서대로 정렬
// patterns는 weight 내림차순 정렬 권장
static const BossPhase s_boss_a_phases[] = {
    {
        .hpThreshold = 1.0f,    // 100%~
        .patternArrCount = 4,
        .patternArr = (const PhasePattern[]){
            { 
                .weight = 0.3f, 
                .patternCount = 1,
                .patterns = {
                    {
                        .type = PATTERN_AIMED,
                        .desc = {
                            .duration = 1.0f,
                            .actionCount = 3,
                            .interval = 0.3f,
                            .attack = {
                                .shape = ATTACK_SHAPE_SINGLE,
                                .attackType = PROJECTILE_PLASMA
                            }
                        }
                    }
                }
            },
            { 
                .weight = 0.3f, 
                .patternCount = 1,
                .patterns = {
                    {
                        .type = PATTERN_ROTATING,
                        .desc = {
                            .duration = 1.0f,
                            .interval = BOSS_BURST_INTERVAL,
                            .patternData = {
                                .rotating = { .speed = BOSS_ROTATION_SPEED * 0.7f, .angle = 0.0f }
                            },
                            .attack = {
                                .shape = ATTACK_SHAPE_CIRCULAR,
                                .attackType = PROJECTILE_BOSS_BULLET,
                                .shapeData = { .circular = { .count = 12 } }
                            }
                        }
                    }
                }
            },
            { 
                .weight = 0.2f, 
                .patternCount = 1,
                .patterns = {
                    {
                        .type = PATTERN_ROTATING,
                        .desc = {
                            .duration = 1.0f,
                            .interval = BOSS_BURST_INTERVAL * 0.8f,
                            .patternData = {
                                .rotating = { .speed = BOSS_ROTATION_SPEED * 1.2f, .angle = 0.0f }
                            },
                            .attack = {
                                .shape = ATTACK_SHAPE_CIRCULAR,
                                .attackType = PROJECTILE_BOSS_BULLET,
                                .shapeData = { .circular = { .count = 24 } }
                            }
                        }
                    }
                }
            },
            { 
                .weight = 0.2f, 
                .patternCount = 1,
                .patterns = {
                    {
                        .type = PATTERN_FIXED,
                        .desc = {
                            .duration = 5.0f,
                            .actionCount = 2,
                            .interval = 3.0f,
                            .patternData = {
                                .fixed = { .angle = DEG2RAD(90.0f) } // 아래 방향
                            },
                            .attack = {
                                .shape = ATTACK_SHAPE_SINGLE,
                                .attackType = BEAM_FIXED,
                            }
                        }
                    }
                }
            }
        }
    },
    {
        .hpThreshold = 0.5f,    // 50%~
        .patternArrCount = 2,
        .patternArr = (const PhasePattern[]){
            { 
                .weight = 0.5f, 
                .patternCount = 1,
                .patterns = {
                    {
                        .type = PATTERN_ROTATING,
                        .desc = {
                            .duration = 2.0f,
                            .interval = BOSS_BURST_INTERVAL * 0.8f,
                            .patternData = {
                                .rotating = { .speed = BOSS_ROTATION_SPEED * 1.2f, .angle = 0.0f }
                            },
                            .attack = {
                                .shape = ATTACK_SHAPE_CIRCULAR,
                                .attackType = PROJECTILE_BOSS_BULLET,
                                .shapeData = { .circular = { .count = 24 } }
                            }
                        }
                    }
                }
            },
            { 
                .weight = 0.5f, 
                .patternCount = 2,
                .patterns = {
                    {
                        .type = PATTERN_AIMED,
                        .desc = {
                            .duration = 2.0f,
                            .actionCount = 5,
                            .interval = 0.2f,
                            .attack = {
                                .shape = ATTACK_SHAPE_SINGLE,
                                .attackType = PROJECTILE_PLASMA
                            }
                        }
                    },
                    {
                        .type = PATTERN_ROTATING,
                        .desc = {
                            .duration = 2.0f,
                            .interval = BOSS_BURST_INTERVAL * 0.6f,
                            .patternData = {
                                .rotating = { .speed = BOSS_ROTATION_SPEED * -1.5f, .angle = 0.0f }
                            },
                            .attack = {
                                .shape = ATTACK_SHAPE_CIRCULAR,
                                .attackType = PROJECTILE_BOSS_BULLET,
                                .shapeData = { .circular = { .count = 16 } }
                            }
                        }
                    }
                }
            }
        }
    }
};

// 전체 설계도
const BossDesc BossA_Desc = {
    .imageAsset = ASSET_BOSS_A,
    .imageRotationSpeed = 45.0f,
    .maxHp = BOSS_MAX_HP,
    .phaseCount = 2,
    .phases = s_boss_a_phases
};