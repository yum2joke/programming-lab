#include "aimed.h"

#include "game/attacks/attack.h"
#include "game/patterns/pattern.h"

#include <math.h>
#include <stdlib.h>

typedef struct {
    int currentCount;
    float fireCooldown;
    float elapsedTime;
    PatternDesc desc;
} AimedState;

static PatternStatus AimedPattern_Update(Pattern* self, float deltaTime, float x, float y)
{
    if (!self || !self->state)
    {
        return PATTERN_FINISHED;
    }

    AimedState* state = (AimedState*)self->state;
    state->elapsedTime += deltaTime;

    if (state->elapsedTime >= state->desc.duration)
    {
        return PATTERN_FINISHED;
    }

    state->fireCooldown -= deltaTime;

    // 설정된 격발 횟수(count)만큼 간격(interval)을 두고 조준 사격
    if (state->fireCooldown <= 0.0f && state->currentCount < state->desc.actionCount)
    {
        state->fireCooldown += state->desc.interval;

        float angle = Pattern_GetAngleToPlayer(x, y);
        Attack_Execute(x, y, angle, &state->desc.attack);
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

    state->elapsedTime = 0.0f;
    state->currentCount = 0;
    state->fireCooldown = 0.0f; // 생성 즉시 첫 발 장전
    state->desc = *desc;

    p->state = state;
    p->update = AimedPattern_Update;
    p->destroy = Pattern_Destroy;

    return p;
}