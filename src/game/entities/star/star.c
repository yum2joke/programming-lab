#include "star.h"
#include "config.h"
#include <stdlib.h>

typedef struct {
    float x, y;
    int size;
} Star;

static Star s_stars[STAR_COUNT];

void Star_Init(RECT clientRect)
{
    for (int i = 0; i < STAR_COUNT; ++i)
    {
        s_stars[i].x = (float)(rand() % clientRect.right);
        s_stars[i].y = (float)(rand() % clientRect.bottom);
        s_stars[i].size = (rand() % STAR_MAX_SIZE) + 1;
    }
}

void Star_Update(float deltaTime, RECT clientRect)
{
    for (int i = 0; i < STAR_COUNT; ++i)
    {
        s_stars[i].y -= STAR_SPEED * deltaTime;
        if (s_stars[i].y < 0)
        {
            s_stars[i].y = (float)clientRect.bottom;
            s_stars[i].x = (float)(rand() % clientRect.right);
            s_stars[i].size = (rand() % STAR_MAX_SIZE) + 1;
        }
    }
}

void Star_Render(HDC hdc)
{
    HBRUSH hStarBrush = CreateSolidBrush(STAR_COLOR);
    for (int i = 0; i < STAR_COUNT; ++i)
    {
        RECT starRect = {
            (int)s_stars[i].x,
            (int)s_stars[i].y,
            (int)s_stars[i].x + s_stars[i].size,
            (int)s_stars[i].y + s_stars[i].size
        };
        FillRect(hdc, &starRect, hStarBrush);
    }
    DeleteObject(hStarBrush);
}