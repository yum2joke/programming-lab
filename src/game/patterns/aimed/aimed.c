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

    // interval 간격으로 actionCount 횟수만큼 격발
    // actionCount <= 0이면 무제한 격발
    if (state->fireCooldown <= 0.0f && (state->desc.actionCount <= 0 || state->currentCount < state->desc.actionCount))
    {
        state->fireCooldown += state->desc.interval;

        float angle = Pattern_GetAngleToPlayer(x, y);
        // TODO: update 시그니처에 ownerId 추가 시 실제 주체의 ID로 교체 필요 (임시 0)
        Attack_Execute(0, x, y, angle, &state->desc.attack);
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