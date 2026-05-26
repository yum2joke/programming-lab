#include "rotating.h"

#include "game/attacks/attack.h"
#include "game/patterns/pattern.h"

#include <stdlib.h>

typedef struct {
    float angle;
    float fireCooldown;
    float elapsedTime;
    PatternDesc desc;
} RotatingState;

static PatternStatus RotatingPattern_Update(Pattern* self, float deltaTime, float x, float y)
{
    if (!self || !self->state)
    {
        return PATTERN_FINISHED;
    }

    RotatingState* state = (RotatingState*)self->state;

    state->elapsedTime += deltaTime;
    if (state->elapsedTime >= state->desc.duration)
    {
        return PATTERN_FINISHED;
    }

    state->angle += state->desc.speed * deltaTime;
    state->fireCooldown -= deltaTime;

    if (state->fireCooldown <= 0.0f)
    {
        state->fireCooldown += state->desc.interval;

        Attack_Execute(x, y, state->angle, &state->desc.attack);
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
    state->fireCooldown = desc->interval;
    state->elapsedTime = 0.0f;
    state->desc = *desc;

    p->state = state;
    p->update = RotatingPattern_Update;
    p->destroy = Pattern_Destroy;

    return p;
}