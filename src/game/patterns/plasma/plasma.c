#include "plasma.h"
#include "game/patterns/pattern.h"
#include "game/attacks/attack.h"
#include "game/entities/projectiles/projectile_types.h"
#include <stdlib.h>

typedef struct {
    float duration;
    float elapsedTime;
    bool hasFired;
} PlasmaState;

static PatternStatus PlasmaPattern_Update(Pattern* self, float deltaTime, float x, float y)
{
    if (!self || !self->state)
    {
        return PATTERN_FINISHED;
    }

    PlasmaState* state = (PlasmaState*)self->state;

    state->elapsedTime += deltaTime;
    if (state->elapsedTime >= state->duration)
    {
        return PATTERN_FINISHED;
    }

    if (!state->hasFired)
    {
        Attack_SingleShot_AimToPlayer(x, y, PROJECTILE_PLASMA);
        state->hasFired = true;
    }
   
    return PATTERN_RUNNING;
}

Pattern* PlasmaPattern_Create(float duration)
{
    Pattern* p = (Pattern*)malloc(sizeof(Pattern));
    if (!p) return NULL;

    PlasmaState* state = (PlasmaState*)malloc(sizeof(PlasmaState));
    if (!state)
    {
        free(p);
        return NULL;
    }

    state->duration = duration;
    state->elapsedTime = 0.0f;
    state->hasFired = false;

    p->state = state;
    p->update = PlasmaPattern_Update;
    p->destroy = Pattern_Destroy;

    return p;
}
