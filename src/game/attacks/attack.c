#include "attack.h"

#include "config.h"
#include "game/entities/projectiles/projectile.h"
// #include beam

#include <math.h>


#ifndef PI
#define PI 3.1415926535f
#endif

void Attack_SingleShot(float originX, float originY, float dirX, float dirY, AttackEntityType attackType)
{
    Projectile_Spawn(attackType, originX, originY, dirX, dirY);
}

void Attack_CircularBurst(float originX, float originY, int bulletCount, float startAngle, AttackEntityType attackType)
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
        Projectile_Spawn(attackType, originX, originY, dirX, dirY);
    }
}

void Attack_Execute(float originX, float originY, float centerAngle, const AttackDesc* desc)
{
    float dirX = 0.0f;
    float dirY = 0.0f;

    switch (desc->shape)
    {
        case ATTACK_SHAPE_SINGLE:
            dirX = cosf(centerAngle);
            dirY = sinf(centerAngle);
            Attack_SingleShot(originX, originY, dirX, dirY, desc->attackType);
            break;
        case ATTACK_SHAPE_CIRCULAR:
            Attack_CircularBurst(originX, originY, desc->shapeData.circular.count, centerAngle, desc->attackType);
            break;
        case ATTACK_SHAPE_SPREAD:
            // TODO: 추후 부채꼴 발사 로직 연동
            break;
    }
}