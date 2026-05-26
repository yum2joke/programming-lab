#include "attack.h"
#include "game/entities/projectiles/projectile.h"
#include "game/entities/actors/player/player.h"
#include "config.h"
#include <math.h>

#ifndef PI
#define PI 3.1415926535f
#endif

void Attack_SingleShot(float originX, float originY, float dirX, float dirY, ProjectileType projType)
{
    Projectile_Spawn(projType, originX, originY, dirX, dirY);
}

void Attack_SingleShot_AimToPlayer(float originX, float originY, ProjectileType projType)
{
    float targetX = Player_GetX() + (PLAYER_SIZE / 2.0f);
    float targetY = Player_GetY() + (PLAYER_SIZE / 2.0f);
    float angle = atan2f(targetY - originY, targetX - originX);

    float dirX = cosf(angle);
    float dirY = sinf(angle);
    Projectile_Spawn(projType, originX, originY, dirX, dirY);
}

void Attack_CircularBurst(float originX, float originY, int bulletCount, float startAngle, ProjectileType projType)
{
    if (bulletCount <= 0)
    {
        return;
    }

    for (int i = 0; i < bulletCount; i++)
    {
        float angle = startAngle + (i * (2.0f * PI / bulletCount));
        float dirX = cosf(angle);
        float dirY = sinf(angle);
        Projectile_Spawn(projType, originX, originY, dirX, dirY);
    }
}