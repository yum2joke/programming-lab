#include <math.h>

#include "attack.h"

#ifndef PI
#define PI 3.1415926535f
#endif

void Attack_SingleShot(float originX, float originY, float dirX, float dirY, void (*createFunc)(float, float, float, float))
{
    if (createFunc)
    {
        createFunc(originX, originY, dirX, dirY);
    }
}

void Attack_CircularBurst(float originX, float originY, int bulletCount, float startAngle, void (*createFunc)(float, float, float, float))
{
    if (!createFunc || bulletCount <= 0)
    {
        return;
    }

    for (int i = 0; i < bulletCount; i++)
    {
        float angle = startAngle + (i * (2.0f * PI / bulletCount));
        float dirX = cosf(angle);
        float dirY = sinf(angle);
        createFunc(originX, originY, dirX, dirY);
    }
}