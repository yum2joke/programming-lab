#pragma once

#include <windows.h>
#include <stdbool.h>
#include "game/collision/collision_types.h"
#include "game/attacks/attack_types.h"

typedef struct Projectile Projectile;

// 함수 포인터
typedef void (*ProjectileUpdateFunc)(Projectile* self, float deltaTime, RECT clientRect);
typedef void (*ProjectileRenderFunc)(const Projectile* self, HDC hdc);

// 투사체 정적 속성 템플릿. 추상클래스 역할
typedef struct {
    CollisionLayer layer;
    int mask;
    float speed;
    COLORREF color;
    int size;
    
    // 함수 포인터. 순수가상함수 역할
    ProjectileUpdateFunc update;
    ProjectileRenderFunc render;
} ProjectileDesc;

// 투사체 구조체.
// -> 연산자로 직접접근을 허용하기 위해 헤더파일에 선언.
struct Projectile {
    // 동적 데이터
    bool active;
    float x, y;

    // 정적 속성 템플릿
    CollisionLayer layer;
    int mask;
    COLORREF color;
    int size;
    float speed;
    ProjectileUpdateFunc update;
    ProjectileRenderFunc render;

    // 투사체 종류별 고유 데이터
    union {
        struct { float dx, dy; } linear;    // 직선 투사체
        // TODO: 유도 투사체 (Tracking / Guided) 구현하기
    } data;
};