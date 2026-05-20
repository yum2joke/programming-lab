#pragma once

#include <windows.h>
#include <stdbool.h>

#include "game/collision/collision_types.h"

void Bullet_Init(void);
void Bullet_Update(float deltaTime, RECT clientRect);
void Bullet_Render(HDC hdc);

// 총알 발사 요청 함수
void Bullet_Fire(CollisionLayer layer, float startX, float startY, float dirX, float dirY);

// 총알 상태 접근 함수
bool Bullet_IsActive(int index);
void Bullet_GetPosition(int index, LONG* x, LONG* y);
void Bullet_Deactivate(int index);
CollisionLayer Bullet_GetLayer(int index);