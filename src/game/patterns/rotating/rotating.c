#include "rotating.h"

#include "game/attacks/attack.h"
#include "game/patterns/pattern.h"

#include <stdlib.h>

typedef struct {
    float angle;
    int currentCount;
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

    // 회전하는 각도 계산
    state->angle += state->desc.patternData.rotating.speed * deltaTime;

    state->fireCooldown -= deltaTime;

    // interval 간격으로 actionCount 횟수만큼 격발
    // actionCount <= 0이면 무제한 격발
    if (state->fireCooldown <= 0.0f && (state->desc.actionCount <= 0 || state->currentCount < state->desc.actionCount))
    {
        state->fireCooldown += state->desc.interval;

        // TODO: update 시그니처에 ownerId 추가 시 실제 주체의 ID로 교체 필요 (임시 0)
        Attack_Execute(0, x, y, state->angle, &state->desc.attack);
        state->currentCount++;
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

    state->angle = desc->patternData.rotating.angle;
    state->currentCount = 0;
    state->fireCooldown = desc->interval;
    state->elapsedTime = 0.0f;
    state->desc = *desc;

    p->state = state;
    p->update = RotatingPattern_Update;
    p->destroy = Pattern_Destroy;

    return p;
}