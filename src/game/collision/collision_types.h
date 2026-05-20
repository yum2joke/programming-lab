#pragma once

#include <windows.h>

// 충돌 객체 레이어
// LayerMask 이용. 0000, 0001, 0010, 0100... 쉬프트되는 순서대로 저장. 최대 32개(int)
typedef enum {
    LAYER_NONE           = 0,
    LAYER_PLAYER         = 1 << 0, // 1
    LAYER_PLAYER_BULLET  = 1 << 1, // 2
    LAYER_BOSS           = 1 << 2, // 4
    LAYER_BOSS_BULLET    = 1 << 3, // 8
    // TODO: LAYER_ENEMY, LAYER_ENEMY_BULLET, LAYER_ITEM 등 추가
} CollisionLayer;

// 충돌 검사를 위한 범용 객체
typedef struct {
    CollisionLayer layer; // 자신의 레이어
    int mask;             // 충돌 대상 레이어 마스크
    RECT rect;            // 충돌 영역 (AABB)
    int source_index;     // 원본 객체 인덱스
} Collidable;