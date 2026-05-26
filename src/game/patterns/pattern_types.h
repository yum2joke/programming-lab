#pragma once

#include <windows.h>

// 패턴의 종류
typedef enum {
    PATTERN_NONE,
    PATTERN_ROTATING_SLOW_12,
    PATTERN_ROTATING_FAST_24,
    PATTERN_PLASMA,
    PATTERN_TYPE_COUNT
} PatternType;

// 패턴 업데이트 함수의 반환 타입
typedef enum {
    PATTERN_RUNNING,
    PATTERN_FINISHED
} PatternStatus;

// 패턴 업데이트 함수포인터
typedef PatternStatus (*PatternUpdateFunc)(struct Pattern* self, float deltaTime, float x, float y);

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