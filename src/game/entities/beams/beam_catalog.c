#include "beam_catalog.h"

#include "config.h"
#include "game/entities/actors/actor_manager.h"
#include "gdiplus_c.h"

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
    float endX = self->startX + self->dirX * 2500.0f;
    float endY = self->startY + self->dirY * 2500.0f;

    GpGraphics* graphics = NULL;
    GdipCreateFromHDC(hdc, &graphics);
    GdipSetSmoothingMode(graphics, SmoothingModeAntiAlias);

    if (self->state == BEAM_STATE_CHARGING)
    {
        // 경고 범위
        GpPen* warningPen = NULL;
        GdipCreatePen1(MAKE_ARGB(70, 255, 0, 0), self->thickness, UnitPixel, &warningPen);
        GdipDrawLine(graphics, warningPen, self->startX, self->startY, endX, endY);
        GdipDeletePen(warningPen);

        // 경고선
        GpPen* warningLinePen = NULL;
        GdipCreatePen1(MAKE_ARGB(255, 255, 0, 0), 2.0f, UnitPixel, &warningLinePen);
        GdipDrawLine(graphics, warningLinePen, self->startX, self->startY, endX, endY);
        GdipDeletePen(warningLinePen);
    }
    else if (self->state == BEAM_STATE_FIRING)
    {
        // 빔의 진행 방향에 수직인 벡터 계산 (dirX, dirY는 단위 벡터)
        float perpX = -self->dirY;
        float perpY = self->dirX;
        
        // 선의 중심축에서 양쪽 가장자리(두께의 절반)로 떨어진 두 점
        // 이 두 점을 기준으로 그라데이션 축이 설정됩니다.
        GpPointF p1 = { self->startX - perpX * (self->thickness / 2.0f), self->startY - perpY * (self->thickness / 2.0f) };
        GpPointF p2 = { self->startX + perpX * (self->thickness / 2.0f), self->startY + perpY * (self->thickness / 2.0f) };
        
        GpLineGradient* brush = NULL;
        // 0은 GpWrapModeTile을 의미합니다. 초기 색상은 아래 Blend에서 덮어씌워집니다.
        GdipCreateLineBrush(&p1, &p2, 0, 0, 0, &brush);

        // 중심은 하얀색, 양쪽 외곽은 보라색인 3점 보간 설정
        ARGB colors[6] = {
            MAKE_ARGB(100, 200, 140, 255), // 왼쪽 외곽 (보라)
            MAKE_ARGB(255, 200, 140, 255), // 왼쪽 외곽 (보라)
            MAKE_ARGB(255, 255, 255, 255), // 중심 (하양)
            MAKE_ARGB(255, 255, 255, 255), // 중심 (하양)
            MAKE_ARGB(255, 200, 140, 255), // 왼쪽 외곽 (보라)
            MAKE_ARGB(100, 200, 140, 255)  // 오른쪽 외곽 (보라)
        };
        REAL positions[6] = { 0.0f, 0.2f, 0.4f, 0.6f, 0.8f, 1.0f };
        GdipSetLinePresetBlend(brush, colors, positions, 6);

        GpPen* pen = NULL;
        // 브러시를 기반으로 펜을 생성합니다. (GdipCreatePen2)
        GdipCreatePen2((GpBrush*)brush, self->thickness, UnitPixel, &pen);

        GdipDrawLine(graphics, pen, self->startX, self->startY, endX, endY);

        GdipDeletePen(pen);
        GdipDeleteBrush((GpBrush*)brush);
    }
    
    GdipDeleteGraphics(graphics);
}

// --- 설계도 ---

static const BeamDesc s_fixed_beam_desc = {
    .layer = LAYER_ENEMY_ATTACK,
    .mask = LAYER_PLAYER,
    .color = RGB(200, 140, 255),
    .thickness = 80.0f,
    .shape = SHAPE_CAPSULE,
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