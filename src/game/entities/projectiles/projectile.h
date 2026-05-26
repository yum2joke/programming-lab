#pragma once

#include <windows.h>
#include "projectile_types.h"

// --- 투사체 전체관리자 함수 ---

void Projectile_Init(void);
void Projectile_Update(float deltaTime, RECT clientRect);
void Projectile_Render(HDC hdc);

void Projectile_Spawn(ProjectileType type, float startX, float startY, float dirX, float dirY);

// --- 외부 모듈 상호작용 함수 ---

int Projectile_GetPoolSize(void);

// 해당 인덱스의 투사체 포인터 리턴
const Projectile* Projectile_GetFromPool(int index);

// 투사체 비활성화
void Projectile_Deactivate(int index);