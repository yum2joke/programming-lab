#pragma once

#include <stdbool.h>

#define ACTOR_ID_PLAYER 0
#define ACTOR_ID_INVALID -1

// player가 워낙 특수해서, id를 따로 빼놨음.
// 힘들여서 actor_manager를 만들었지만,
// 격발의 주체(ownerId)를 계속 추적해야 하는 상황이 아니면- 예를 들어 플레이어 조준/트래킹?
// 굳이 억지로는 사용안하는 게 좋아보임. 플레이어 말고 조준할 일도 없을 거 같고, 리팩토링도 슬슬 버겁고.. 두고봐야할듯

void ActorManager_Init(void);
int ActorManager_Register(void);
void ActorManager_RegisterPlayer(void);
void ActorManager_UpdatePosition(int id, float x, float y);
void ActorManager_Remove(int id);

// 주소호출로 XY 전달, 리턴 false면 해당 actor 비활성화된것.
bool ActorManager_GetPosition(int id, float* outX, float* outY);