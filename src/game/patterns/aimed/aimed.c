#include "aimed.h"
#include "game/patterns/pattern.h"
#include "game/attacks/attack.h"
#include <stdlib.h>
#include <math.h>

typedef struct {
    float duration;
    float elapsedTime;
    AttackEntityType attackType;
    int maxCount;
    int currentCount;
    float fireInterval;
    float fireCooldown;
} AimedState;

static PatternStatus AimedPattern_Update(Pattern* self, float deltaTime, float x, float y)
{
    if (!self || !self->state)
    {
        return PATTERN_FINISHED;
    }

    AimedState* state = (AimedState*)self->state;
    state->elapsedTime += deltaTime;

    if (state->elapsedTime >= state->duration)
    {
        return PATTERN_FINISHED;
    }

    state->fireCooldown -= deltaTime;

    // 설정된 격발 횟수(maxCount)만큼 간격(fireInterval)을 두고 조준 사격
    if (state->fireCooldown <= 0.0f && state->currentCount < state->maxCount)
    {
        state->fireCooldown += state->fireInterval;

        float angle = Pattern_GetAngleToPlayer(x, y);
        float dirX = cosf(angle);
        float dirY = sinf(angle);

        Attack_SingleShot(x, y, dirX, dirY, state->attackType);
        state->currentCount++;
    }
   
    return PATTERN_RUNNING;
}

Pattern* AimedPattern_Create(const PatternDesc* desc)
{
    Pattern* p = (Pattern*)malloc(sizeof(Pattern));
    if (!p) return NULL;

    AimedState* state = (AimedState*)malloc(sizeof(AimedState));
    if (!state)
    {
        free(p);
        return NULL;
    }

    state->duration = desc->duration;
    state->elapsedTime = 0.0f;
    state->attackType = desc->attackType;
    state->maxCount = desc->count;
    state->currentCount = 0;
    state->fireInterval = desc->interval;
    state->fireCooldown = 0.0f;

    p->state = state;
    p->update = AimedPattern_Update;
    p->destroy = Pattern_Destroy;

    return p;
}