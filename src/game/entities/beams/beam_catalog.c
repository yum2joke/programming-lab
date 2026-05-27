#include "beam_catalog.h"

#include "config.h"
#include "game/entities/actors/actor_manager.h"

#include <stddef.h>

// --- 업데이트 함수 ---

static void UpdateBeam_Fixed(Beam* self, float deltaTime, RECT clientRect)
{
    float newX, newY;
    if (ActorManager_GetPosition(self->ownerId, &newX, &newY))
    {
        self->startX = newX;
        self->startY = newY;
    }
    else
    {
        // 발사 주체 파괴 시, 빔도 소멸
        self->active = false;
        return;
    }

    self->elapsedTime += deltaTime;

    if (self->state == BEAM_STATE_CHARGING)
    {
        // 차징 시간 종료 시- 발사 상태로 전환
        if (self->elapsedTime >= self->chargeTime)
        {
            self->state = BEAM_STATE_FIRING;
        }
    }
    else if (self->state == BEAM_STATE_FIRING)
    {
        // 차징 시간 + 본 발사 시간이 모두 지나면 소멸
        if (self->elapsedTime >= self->chargeTime + self->duration)
        {
            self->active = false;
        }
    }
}

// --- 렌더링 함수 ---

static void RenderBeam_Default(const Beam* self, HDC hdc)
{
    int endX = (int)(self->startX + self->dirX * 2500.0f);
    int endY = (int)(self->startY + self->dirY * 2500.0f);

    if (self->state == BEAM_STATE_CHARGING)
    {
        // 경고 범위
        HPEN hWarningPen = CreatePen(PS_SOLID, (int)self->thickness, RGB(110, 50, 50));
        HPEN hOldPen = (HPEN)SelectObject(hdc, hWarningPen);
        MoveToEx(hdc, (int)self->startX, (int)self->startY, NULL);
        LineTo(hdc, endX, endY);

        // 경고선
        HPEN hWarningLinePen = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
        SelectObject(hdc, hWarningLinePen);
        MoveToEx(hdc, (int)self->startX, (int)self->startY, NULL);
        LineTo(hdc, endX, endY);

        SelectObject(hdc, hOldPen);
        DeleteObject(hWarningPen);
        DeleteObject(hWarningLinePen);
    }
    else if (self->state == BEAM_STATE_FIRING)
    {
        // 외곽 광원
        HPEN hOuterPen = CreatePen(PS_SOLID, (int)self->thickness, self->color);
        HPEN hOldPen = (HPEN)SelectObject(hdc, hOuterPen);
        MoveToEx(hdc, (int)self->startX, (int)self->startY, NULL);
        LineTo(hdc, endX, endY);

        // 외곽 광원2
        HPEN hOuter2Pen = CreatePen(PS_SOLID, (int)(self->thickness * 0.8f), RGB(200, 180, 255));
        SelectObject(hdc, hOuter2Pen);
        MoveToEx(hdc, (int)self->startX, (int)self->startY, NULL);
        LineTo(hdc, endX, endY);

        // 중심 코어
        HPEN hInnerPen = CreatePen(PS_SOLID, (int)(self->thickness * 0.6f), RGB(255, 255, 255));
        SelectObject(hdc, hInnerPen);
        MoveToEx(hdc, (int)self->startX, (int)self->startY, NULL);
        LineTo(hdc, endX, endY);

        SelectObject(hdc, hOldPen);
        DeleteObject(hOuterPen);
        DeleteObject(hOuter2Pen);
        DeleteObject(hInnerPen);
    }
}

// --- 설계도 ---

static const BeamDesc s_fixed_beam_desc = {
    .layer = LAYER_BOSS_BULLET,
    .mask = LAYER_PLAYER,
    .color = RGB(200, 140, 255),
    .thickness = 80.0f,
    .chargeTime = 1.0f,
    .duration = 2.0f,
    .update = UpdateBeam_Fixed,
    .render = RenderBeam_Default
};

// --- 설계도 배열 ---
static const BeamDesc* s_beam_catalog[ATTACK_TYPE_COUNT] = {
    [BEAM_FIXED] = &s_fixed_beam_desc,
    //[BEAM_ROTATE] = &s_rotated_beam_desc,
    //[BEAM_PLAYER_TRACKING] = &s_player_tracking_beam_desc,
};

const BeamDesc* BeamCatalog_GetDesc(AttackEntityType type)
{
    if (type < PROJECTILE_TYPE_COUNT || type >= ATTACK_TYPE_COUNT)
    {
        return NULL;
    }

    return s_beam_catalog[type];
}