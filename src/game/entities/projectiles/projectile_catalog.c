#include "projectile_catalog.h"
#include "config.h"

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

// 사각형 렌더링
static void RenderProjectile_Default(const Projectile* self, HDC hdc)
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

// --- 설계도 ---

// 플레이어 기본 총알
static const ProjectileDesc s_player_bullet_desc = {
    .layer = LAYER_PLAYER_BULLET,
    .mask = LAYER_BOSS,
    .speed = BULLET_SPEED,
    .color = BULLET_COLOR,
    .size = BULLET_SIZE,
    .update = UpdateProjectile_Linear,
    .render = RenderProjectile_Default
};

// 보스 기본 총알
static const ProjectileDesc s_boss_bullet_desc = {
    .layer = LAYER_BOSS_BULLET,
    .mask = LAYER_PLAYER,
    .speed = BOSS_BULLET_SPEED,
    .color = BOSS_BULLET_COLOR,
    .size = BULLET_SIZE,
    .update = UpdateProjectile_Linear,
    .render = RenderProjectile_Default
};

// --- 설계도 배열 ---

// 모든 투사체 설계도가 담긴 배열
static const ProjectileDesc* s_projectile_catalog[PROJECTILE_TYPE_COUNT] = {
    [PROJECTILE_PLAYER_BULLET] = &s_player_bullet_desc,
    [PROJECTILE_BOSS_BULLET] = &s_boss_bullet_desc,
};

const ProjectileDesc* ProjectileCatalog_GetDesc(ProjectileType type)
{
    if (type < 0 || type >= PROJECTILE_TYPE_COUNT)
    {
        return NULL;
    }

    return s_projectile_catalog[type];
}