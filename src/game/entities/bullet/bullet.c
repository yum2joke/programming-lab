#include "bullet.h"
#include "config.h"
#include <stdbool.h>

typedef struct {
    float x, y;
    float dx, dy; // 방향 벡터
    bool active;
} Bullet;

static Bullet s_bullets[BULLET_MAX_COUNT];

void Bullet_Init(void)
{
    for (int i = 0; i < BULLET_MAX_COUNT; ++i)
    {
        s_bullets[i].active = false;
    }
}

void Bullet_Update(float deltaTime, RECT clientRect)
{
    for (int i = 0; i < BULLET_MAX_COUNT; ++i)
    {
        if (s_bullets[i].active)
        {
            s_bullets[i].x += s_bullets[i].dx * BULLET_SPEED * deltaTime;
            s_bullets[i].y += s_bullets[i].dy * BULLET_SPEED * deltaTime;

            if (s_bullets[i].x < 0 || s_bullets[i].x > clientRect.right ||
                s_bullets[i].y < 0 || s_bullets[i].y > clientRect.bottom)
            {
                s_bullets[i].active = false;
            }
        }
    }
}

void Bullet_Render(HDC hdc)
{
    HBRUSH hBulletBrush = CreateSolidBrush(BULLET_COLOR);
    for (int i = 0; i < BULLET_MAX_COUNT; ++i)
    {
        if (s_bullets[i].active)
        {
            RECT bulletRect = {
                (int)s_bullets[i].x - BULLET_SIZE / 2,
                (int)s_bullets[i].y - BULLET_SIZE / 2,
                (int)s_bullets[i].x + BULLET_SIZE / 2,
                (int)s_bullets[i].y + BULLET_SIZE / 2
            };
            FillRect(hdc, &bulletRect, hBulletBrush);
        }
    }
    DeleteObject(hBulletBrush);
}

void Bullet_Fire(float startX, float startY, float dirX, float dirY)
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
            break;
        }
    }
}