#pragma once

#include <windows.h>
#include "game/attacks/attack_types.h"

// 패턴의 종류
typedef enum {
    PATTERN_NONE,
    PATTERN_ROTATING,
    PATTERN_AIMED,
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

// 패턴 생성 및 실행을 위한 범용 데이터 명세서 (DDD 적용)
typedef struct {
    float duration;
    AttackEntityType attackType;

    // 범용 매개변수 풀
    int count;           // 생성 갯수, 격발 횟수 등
    float interval;      // 발사 간격
    float speed;         // 회전 속도 등 범용 속도
    float angle;         // 범용 각도
} PatternDesc;