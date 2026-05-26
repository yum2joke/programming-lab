#pragma once

#include "game/patterns/pattern_types.h"

// 생성할 보스의 종류
typedef enum {
    BOSS_TYPE_A,
    // BOSS_TYPE_B, ...
    BOSS_TYPE_COUNT
} BossType;

// 페이즈를 구성하는 개별 패턴
typedef struct {
    PatternType type;   // 패턴 종류
    float duration;     // 패턴 지속시간 (초)
    float weight;       // 선택될 확률 가중치 (전체 합 = 1.0)
} PhasePattern;

// 보스의 단일 페이즈 정의
typedef struct {
    float hpThreshold;              // 이 페이즈가 시작되는 HP 비율 (0.5 = 50%)
    int patternCount;               // 패턴 가짓수
    const PhasePattern* patterns;   // 패턴 배열. (weight 기준 내림차순 정렬 권장)
} BossPhase;

// 보스의 완전한 설계도
typedef struct {
    float maxHp;
    int phaseCount;
    const BossPhase* phases; // 페이즈 정의 배열
} BossDesc;