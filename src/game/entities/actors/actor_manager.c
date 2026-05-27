#include "actor_manager.h"

#define MAX_ACTORS 32

// 캐시 친화적인 DOD 구조를 위한 데이터 배열
typedef struct {
    bool active;
    float x;
    float y;
} ActorData;

// 오브젝트 풀
static ActorData s_actors[MAX_ACTORS];

void ActorManager_Init(void)
{
    for (int i = 0; i < MAX_ACTORS; i++)
    {
        s_actors[i].active = false;
        s_actors[i].x = 0.0f;
        s_actors[i].y = 0.0f;
    }
}

int ActorManager_Register(void)
{
    // 0번은 플레이어 고정, 1번부터 발급
    for (int i = 1; i < MAX_ACTORS; i++)
    {
        if (!s_actors[i].active)
        {
            s_actors[i].active = true;
            return i;
        }
    }
    return ACTOR_ID_INVALID;
}

void ActorManager_RegisterPlayer(void)
{
    s_actors[ACTOR_ID_PLAYER].active = true;
}

void ActorManager_UpdatePosition(int id, float x, float y)
{
    if (id >= 0 && id < MAX_ACTORS)
    {
        if (s_actors[id].active)
        {
            s_actors[id].x = x;
            s_actors[id].y = y;
        }
    }
}

void ActorManager_Remove(int id)
{
    if (id >= 0 && id < MAX_ACTORS)
    {
        s_actors[id].active = false;
    }
}

bool ActorManager_GetPosition(int id, float* outX, float* outY)
{
    if (id >= 0 && id < MAX_ACTORS && s_actors[id].active)
    {
        if (outX) *outX = s_actors[id].x;
        if (outY) *outY = s_actors[id].y;
        return true;
    }
    return false;
}