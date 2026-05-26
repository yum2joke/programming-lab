#pragma once

#include "game/patterns/pattern_types.h"

#define MAX_CONCURRENT_PATTERNS 4

// 생성할 보스의 종류
typedef enum {
    BOSS_TYPE_A,
    BOSS_TYPE_COUNT
} BossType;

// 페이즈를 구성하는 개별 패턴
// 콤보를 구성하는 개별 패턴 데이터
typedef struct {
    PatternType type;
    PatternDesc desc;
} PatternData;

// 페이즈를 구성하는 하나의 "행동" (다중 패턴 동시 실행)
typedef struct {
    PatternType type;   // 패턴 종류
    float weight;       // 선택될 확률 가중치 (전체 합 = 1.0)
    PatternDesc desc;   // 패턴에게 넘겨줄 상세 기획 데이터
    int patternCount;   // 동시에 실행할 패턴 가짓수
    PatternData patterns[MAX_CONCURRENT_PATTERNS]; // 실행할 패턴 배열
} PhasePattern;

// 보스의 단일 페이즈 정의
typedef struct {
    float hpThreshold;                  // 이 페이즈가 시작되는 HP 비율 (0.5 = 50%)
    int patternArrCount;                // 패턴 가짓수
    const PhasePattern* patternArr;     // 패턴 배열. (weight 기준 내림차순 정렬 권장)
} BossPhase;

// 보스의 완전한 설계도
typedef struct {
    float maxHp;
    int phaseCount;
    const BossPhase* phases; // 페이즈 정의 배열
} BossDesc;