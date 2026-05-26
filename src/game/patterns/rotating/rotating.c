#include "rotating.h"

#include "game/attacks/attack.h"
#include "game/patterns/pattern.h"

#include <stdlib.h>

typedef struct {
    float angle;
    float rotationSpeed;
    int bulletCount;
    float fireInterval;
    float fireCooldown;
    float duration;
    float elapsedTime;
    AttackEntityType attackType;
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
            state->attackType
        );
    }

    return PATTERN_RUNNING;
}

Pattern* RotatingPattern_Create(const PatternDesc* desc)
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
    state->bulletCount = desc->count;
    state->rotationSpeed = desc->speed;
    state->fireInterval = desc->interval;
    state->fireCooldown = desc->interval;
    state->duration = desc->duration;
    state->elapsedTime = 0.0f;
    state->attackType = desc->attackType;

    p->state = state;
    p->update = RotatingPattern_Update;
    p->destroy = Pattern_Destroy;

    return p;
}