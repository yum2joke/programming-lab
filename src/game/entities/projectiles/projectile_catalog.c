#include "projectile_catalog.h"

#include "config.h"
#include "gdiplus_c.h"

#include <stddef.h> // NULL

// --- 업데이트 함수 ---
// TODO: 파일 분리 필요.

// 직선 투사체 업데이트
static void UpdateProjectile_Linear(Projectile* self, float deltaTime, RECT clientRect)
{
    self->x += self->data.linear.dx * self->speed * deltaTime;
    self->y += self->data.linear.dy * self->speed * deltaTime;

    // 화면 밖으로 나가면 비활성화
    if (self->x < -self->size || self->x > clientRect.right + self->size ||
        self->y < -self->size || self->y > clientRect.bottom + self->size)
    {
        self->active = false;
    }
}

// --- 렌더링 함수 ---
// TODO: 파일 분리 필요.

// 사각형 렌더링 (사용 X)
static void RenderProjectile_Square(const Projectile* self, HDC hdc)
{
    HBRUSH hBrush = CreateSolidBrush(self->color);
    RECT rect = {
        (int)self->x - self->size / 2,
        (int)self->y - self->size / 2,
        (int)self->x + self->size / 2,
        (int)self->y + self->size / 2
    };
    FillRect(hdc, &rect, hBrush);
    DeleteObject(hBrush);
}

// 원 렌더링
static void RenderProjectile_Sphere(const Projectile* self, HDC hdc)
{
    GpGraphics* graphics = NULL;
    GdipCreateFromHDC(hdc, &graphics);
    GdipSetSmoothingMode(graphics, SmoothingModeAntiAlias);

    float renderSize = self->size * 1.5f;
    float x = self->x - renderSize / 2.0f;
    float y = self->y - renderSize / 2.0f;

    // COLORREF를 분리하여 GDI+ ARGB 생성 (가장자리는 투명하게)
    BYTE r = GetRValue(self->color);
    BYTE g = GetGValue(self->color);
    BYTE b = GetBValue(self->color);

    GpPath* path = NULL;
    GdipCreatePath(0, &path); // 0 = FillModeAlternate
    GdipAddPathEllipse(path, x, y, renderSize, renderSize);

    GpPathGradient* brush = NULL;
    GdipCreatePathGradientFromPath(path, &brush);
    
    // 방사형 그라데이션 다중 색상 보간 (0.0 가장자리, 1.0 중심)
    ARGB colors[4] = {
        MAKE_ARGB(180, r, g, b),        // 0.0f (가장자리)
        MAKE_ARGB(255, r, g, b),        // 0.0f (가장자리)
        MAKE_ARGB(255, 255, 255, 255),  // 0.7f: 하양
        MAKE_ARGB(255, 255, 255, 255)   // 1.0f (중심): 화이트 코어
    };
    REAL positions[4] = { 0.0f, 0.3f, 0.7f, 1.0f };
    GdipSetPathGradientPresetBlend(brush, colors, positions, 4);

    GdipFillEllipse(graphics, (GpBrush*)brush, x, y, renderSize, renderSize);

    GdipDeleteBrush((GpBrush*)brush);
    GdipDeletePath(path);
    GdipDeleteGraphics(graphics);
}

// --- 설계도 ---

// 플레이어 기본 총알
static const ProjectileDesc s_player_bullet_desc = {
    .layer = LAYER_PLAYER_BULLET,
    .mask = LAYER_BOSS,
    .speed = BULLET_SPEED,
    .color = BULLET_COLOR,
    .size = BULLET_SIZE,
    .shape = SHAPE_SPHERE,
    .collisionRadius = BULLET_SIZE / 2.0f,
    .passThrough = false,
    .update = UpdateProjectile_Linear,
    .render = RenderProjectile_Sphere
};

// 보스 기본 총알
static const ProjectileDesc s_boss_bullet_desc = {
    .layer = LAYER_ENEMY_ATTACK,
    .mask = LAYER_PLAYER,
    .speed = BOSS_BULLET_SPEED,
    .color = BOSS_BULLET_COLOR,
    .size = BULLET_SIZE,
    .shape = SHAPE_SPHERE,
    .collisionRadius = BULLET_SIZE / 2.0f,
    .passThrough = false,
    .update = UpdateProjectile_Linear,
    .render = RenderProjectile_Sphere
};

// 느린 속도의 대형 플라즈마 구체
static const ProjectileDesc s_plasma_desc = {
    .layer = LAYER_ENEMY_ATTACK,
    .mask = LAYER_PLAYER,
    .speed = BOSS_PLASMA_SPEED,
    .color = BOSS_PLASMA_COLOR,
    .size = BOSS_PLASMA_SIZE,
    .shape = SHAPE_SPHERE,
    .collisionRadius = BOSS_PLASMA_SIZE / 2.0f,
    .passThrough = true,
    .update = UpdateProjectile_Linear,
    .render = RenderProjectile_Sphere
};

// --- 설계도 배열 ---

// 모든 투사체 설계도가 담긴 배열
static const ProjectileDesc* s_projectile_catalog[PROJECTILE_TYPE_COUNT] = {
    [PROJECTILE_PLAYER_BULLET] = &s_player_bullet_desc,
    [PROJECTILE_BOSS_BULLET] = &s_boss_bullet_desc,
    [PROJECTILE_PLASMA] = &s_plasma_desc,
};

const ProjectileDesc* ProjectileCatalog_GetDesc(ProjectileType type)
{
    if (type < 0 || type >= PROJECTILE_TYPE_COUNT)
    {
        return NULL;
    }

    return s_projectile_catalog[type];
}