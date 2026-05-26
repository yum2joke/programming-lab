#include "rotating.h"
#include "game/patterns/pattern.h"
#include "game/attacks/attack.h"
#include "game/entities/projectiles/projectile_types.h"
#include "config.h"
#include <stdlib.h>

typedef struct {
    float angle;
    float rotationSpeed;
    int bulletCount;
    float fireInterval;
    float fireCooldown;
    float duration;
    float elapsedTime;
} RotatingState;

static PatternStatus RotatingPattern_Update(Pattern* self, float deltaTime, float x, float y)
{
    if (!self || !self->state)
    {
        return PATTERN_FINISHED;
    }

    RotatingState* state = (RotatingState*)self->state;

    state->elapsedTime += deltaTime;
    if (state->elapsedTime >= state->duration)
    {
        return PATTERN_FINISHED;
    }

    state->angle += state->rotationSpeed * deltaTime;
    state->fireCooldown -= deltaTime;

    if (state->fireCooldown <= 0.0f)
    {
        state->fireCooldown += state->fireInterval;

        Attack_CircularBurst(
            x,
            y,
            state->bulletCount,
            state->angle,
            PROJECTILE_BOSS_BULLET
        );
    }

    return PATTERN_RUNNING;
}

static Pattern* RotatingPattern_Create(int bulletCount, float rotationSpeed, float fireInterval, float duration)
{
    Pattern* p = (Pattern*)malloc(sizeof(Pattern));
    if (!p) return NULL;

    RotatingState* state = (RotatingState*)malloc(sizeof(RotatingState));
    if (!state)
    {
        free(p);
        return NULL;
    }

    state->angle = 0.0f;
    state->bulletCount = bulletCount;
    state->rotationSpeed = rotationSpeed;
    state->fireInterval = fireInterval;
    state->fireCooldown = fireInterval;
    state->duration = duration;
    state->elapsedTime = 0.0f;

    p->state = state;
    p->update = RotatingPattern_Update;
    p->destroy = Pattern_Destroy;

    return p;
}

// --- 외부로 노출되는 생성 함수들 ---

Pattern* RotatingSlow12Pattern_Create(float duration)
{
    return RotatingPattern_Create(12, BOSS_ROTATION_SPEED * 0.7f, BOSS_BURST_INTERVAL, duration);
}

Pattern* RotatingFast24Pattern_Create(float duration)
{
    return RotatingPattern_Create(24, BOSS_ROTATION_SPEED * 1.2f, BOSS_BURST_INTERVAL * 0.8f, duration);
}