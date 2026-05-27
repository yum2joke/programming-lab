#pragma once

#include "game/attacks/attack_types.h"

#include <windows.h>

// 패턴의 종류
typedef enum {
    PATTERN_NONE,
    PATTERN_ROTATING,
    PATTERN_AIMED,
    PATTERN_FIXED,
    PATTERN_TYPE_COUNT
} PatternType;

// 패턴 업데이트 함수의 반환 타입
typedef enum {
    PATTERN_RUNNING,
    PATTERN_FINISHED
} PatternStatus;

// 패턴 업데이트 함수포인터
typedef PatternStatus (*PatternUpdateFunc)(struct Pattern* self, float deltaTime, float x, float y, int ownerId);

// 패턴 소멸 함수포인터
typedef void (*PatternDestroyFunc)(struct Pattern* self);

// 패턴 기본 구조체. 추상클래스 역할
typedef struct Pattern {
    // 함수 포인터. 순수가상함수 역할
    PatternUpdateFunc update;
    PatternDestroyFunc destroy;

    // 각 패턴의 고유한 상태 데이터를 가리키는 포인터
    void* state;
} Pattern;

// 패턴 설계도
typedef struct {
    // 패턴 매개변수
    int actionCount;    // 격발 횟수 (0이면 무제한 격발)
    float duration;     // 지속 시간
    float interval;     // 발사 간격

    // 패턴 고유 데이터
    union {
        struct { float speed; float angle; } rotating; // 회전 속도 및 시작 각도
        struct { float angle; } fixed;                 // 고정 각도
    } patternData;

    // 발사(모양) 전용 규격서
    AttackDesc attack;
} PatternDesc;