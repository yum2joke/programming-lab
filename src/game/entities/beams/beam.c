#include "beam.h"

#include "beam_catalog.h"
#include "config.h"

static Beam s_beams[BEAM_MAX_COUNT];

void Beam_Init(void)
{
    for (int i = 0; i < BEAM_MAX_COUNT; ++i)
    {
        s_beams[i].active = false;
    }
}

void Beam_Update(float deltaTime, RECT clientRect)
{
    for (int i = 0; i < BEAM_MAX_COUNT; ++i)
    {
        if (s_beams[i].active && s_beams[i].update)
        {
            s_beams[i].update(&s_beams[i], deltaTime, clientRect);
        }
    }
}

void Beam_Render(HDC hdc)
{
    for (int i = 0; i < BEAM_MAX_COUNT; ++i)
    {
        if (s_beams[i].active && s_beams[i].render)
        {
            s_beams[i].render(&s_beams[i], hdc);
        }
    }
}

void Beam_Spawn(int ownerId, AttackEntityType type, float startX, float startY, float dirX, float dirY)
{
    const BeamDesc* desc = BeamCatalog_GetDesc(type);
    if (!desc)
    {
        return;
    }

    for (int i = 0; i < BEAM_MAX_COUNT; ++i)
    {
        if (!s_beams[i].active)
        {
            Beam* b = &s_beams[i];
            b->active = true;
            b->ownerId = ownerId;
            b->startX = startX;
            b->startY = startY;
            b->dirX = dirX;
            b->dirY = dirY;
            b->elapsedTime = 0.0f;

            b->layer = desc->layer;
            b->mask = desc->mask;
            b->color = desc->color;
            b->thickness = desc->thickness;
            b->shape = desc->shape;
            b->chargeTime = desc->chargeTime;
            b->duration = desc->duration;
            b->update = desc->update;
            b->render = desc->render;

            // 차징 시간이 설정돼있지 않으면, 즉발 상태로 초기화
            if (b->chargeTime > 0.0f)
            {
                b->state = BEAM_STATE_CHARGING;
            }
            else
            {
                b->state = BEAM_STATE_FIRING;
            }

            return;
        }
    }
}

int Beam_GetPoolSize(void)
{
    return BEAM_MAX_COUNT;
}

const Beam* Beam_GetFromPool(int index)
{
    return (index >= 0 && index < BEAM_MAX_COUNT) ? &s_beams[index] : NULL;
}