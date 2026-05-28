#pragma once

#include <windows.h>

// 충돌 객체 레이어
// LayerMask 이용. 0000, 0001, 0010, 0100... 쉬프트되는 순서대로 저장. 최대 32개(int)
typedef enum {
    LAYER_NONE           = 0,
    LAYER_PLAYER         = 1 << 0, // 1
    LAYER_PLAYER_BULLET  = 1 << 1, // 2
    LAYER_BOSS           = 1 << 2, // 4
    LAYER_ENEMY_ATTACK   = 1 << 3, // 8
    // TODO: LAYER_ENEMY, LAYER_ENEMY_BULLET, LAYER_ITEM 등 추가
} CollisionLayer;

// 충돌체 형태(Shape)
typedef enum {
    SHAPE_AABB,
    SHAPE_SPHERE,
    SHAPE_CAPSULE
} CollisionShape;

// 원형 충돌 데이터
typedef struct {
    float x;
    float y;
    float radius;
} Sphere;

// 캡슐 충돌 데이터 (두께가 있는 선분)
typedef struct {
    float x, y;       // 시작점
    float dirX, dirY; // 방향 벡터
    float length;     // 길이
    float radius;     // 두께 (반지름)
} Capsule;

// 충돌 검사를 위한 범용 객체
typedef struct {
    CollisionLayer layer; // 자신의 레이어
    int mask;             // 충돌 대상 레이어 마스크
    CollisionShape shape; // 충돌체 형태
    union {
        RECT aabb;        // AABB 충돌 영역
        Sphere sphere;    // 원형 충돌 영역
        Capsule capsule;  // 캡슐 충돌 영역
    } data;
    int source_index;     // 원본 객체 인덱스
} Collidable;