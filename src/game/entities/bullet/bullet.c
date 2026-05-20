#include <stdbool.h>

#include "bullet.h"
#include "config.h"

#include "game/collision/collision_types.h"

typedef struct {
    float x, y;
    float dx, dy; // 방향 벡터
    bool active;
    CollisionLayer layer;
} Bullet;

// 오브젝트 풀링- 총알 객체를 미리 생성하여 재사용
static Bullet s_bullets[BULLET_MAX_COUNT];

// 모든 총알을 비활성 상태로 초기화
void Bullet_Init(void)
{
    for (int i = 0; i < BULLET_MAX_COUNT; ++i)
    {
        s_bullets[i].active = false;
    }
}

// 활성 총알 이동 및 화면 이탈 시 비활성화
void Bullet_Update(float deltaTime, RECT clientRect)
{
    for (int i = 0; i < BULLET_MAX_COUNT; ++i)
    {
        if (s_bullets[i].active)
        {
            float speed = BULLET_SPEED;
            if (s_bullets[i].layer == LAYER_BOSS_BULLET)
            {
                speed = BOSS_BULLET_SPEED;
            }

            s_bullets[i].x += s_bullets[i].dx * speed * deltaTime;
            s_bullets[i].y += s_bullets[i].dy * speed * deltaTime;

            if (s_bullets[i].x < 0 || s_bullets[i].x > clientRect.right ||
                s_bullets[i].y < 0 || s_bullets[i].y > clientRect.bottom)
            {
                s_bullets[i].active = false;
            }
        }
    }
}

// 활성 총알을 알맞은 양식으로 그리기
void Bullet_Render(HDC hdc)
{
    HBRUSH hPlayerBulletBrush = CreateSolidBrush(BULLET_COLOR);
    HBRUSH hBossBulletBrush = CreateSolidBrush(BOSS_BULLET_COLOR);

    for (int i = 0; i < BULLET_MAX_COUNT; ++i)
    {
        if (s_bullets[i].active)
        {
            HBRUSH currentBrush = NULL;
            if (s_bullets[i].layer == LAYER_PLAYER_BULLET)
            {
                currentBrush = hPlayerBulletBrush;
            }
            else if (s_bullets[i].layer == LAYER_BOSS_BULLET)
            {
                currentBrush = hBossBulletBrush;
            }

            if (currentBrush)
            {
                RECT bulletRect = {
                    (int)s_bullets[i].x - BULLET_SIZE / 2,
                    (int)s_bullets[i].y - BULLET_SIZE / 2,
                    (int)s_bullets[i].x + BULLET_SIZE / 2,
                    (int)s_bullets[i].y + BULLET_SIZE / 2
                };
                FillRect(hdc, &bulletRect, currentBrush);
            }
        }
    }
    DeleteObject(hPlayerBulletBrush);
    DeleteObject(hBossBulletBrush);
}

// 오브젝트 풀에서 비활성 총알을 찾아 활성화 및 재사용
void Bullet_Fire(CollisionLayer layer, float startX, float startY, float dirX, float dirY)
{
    for (int i = 0; i < BULLET_MAX_COUNT; ++i)
    {
        if (!s_bullets[i].active)
        {
            s_bullets[i].active = true;
            s_bullets[i].x = startX;
            s_bullets[i].y = startY;
            s_bullets[i].dx = dirX;
            s_bullets[i].dy = dirY;
            s_bullets[i].layer = layer;
            break;
        }
    }
}

bool Bullet_IsActive(int index)
{
    if (index < 0 || index >= BULLET_MAX_COUNT)
    {
        return false;
    }
    return s_bullets[index].active;
}

void Bullet_GetPosition(int index, LONG* x, LONG* y)
{
    if (index >= 0 && index < BULLET_MAX_COUNT && x != NULL && y != NULL)
    {
        *x = (LONG)s_bullets[index].x;
        *y = (LONG)s_bullets[index].y;
    }
}

// 총알을 비활성화하여 풀에 반환
void Bullet_Deactivate(int index)
{
    if (index >= 0 && index < BULLET_MAX_COUNT)
    {
        s_bullets[index].active = false;
    }
}

CollisionLayer Bullet_GetLayer(int index)
{
    if (index >= 0 && index < BULLET_MAX_COUNT)
    {
        return s_bullets[index].layer;
    }
    return LAYER_NONE;
}