#pragma once

#include "game/attacks/attack_types.h"
#include "game/collision/collision_types.h"

#include <stdbool.h>
#include <windows.h>

typedef struct Beam Beam;

typedef enum {
    BEAM_STATE_CHARGING, // 발사 전 경고범위만
    BEAM_STATE_FIRING    // 실제 발사
} BeamState;

// 함수 포인터
typedef void (*BeamUpdateFunc)(Beam* self, float deltaTime, RECT clientRect);
typedef void (*BeamRenderFunc)(const Beam* self, HDC hdc);

// 빔 정적 속성 템플릿. 카탈로그에 등록됨
typedef struct {
    CollisionLayer layer;
    int mask;
    COLORREF color;
    float thickness;    // 빔 두께
    float chargeTime;   // 발사 전 차징시간
    float duration;     // 빔 지속 시간
    
    BeamUpdateFunc update;
    BeamRenderFunc render;
} BeamDesc;

// 화면에 존재하는 빔의 동적 객체 (DOD)
struct Beam {
    bool active;
    int ownerId;          // 발사 주체 식별 ID (위치 동기화용)
    float startX, startY; // 발사 시작점
    float dirX, dirY;     // 발사 방향 벡터
    float elapsedTime;    // 경과 시간
    BeamState state;      // 현재 상태

    // 정적 속성 복사본 (캐시 최적화)
    CollisionLayer layer;
    int mask;
    COLORREF color;
    float thickness;
    float chargeTime;
    float duration;

    BeamUpdateFunc update;
    BeamRenderFunc render;
};