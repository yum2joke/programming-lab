#pragma once

#include "boss_types.h"

#include <stdbool.h>

void Boss_Spawn(BossType type, RECT clientRect);
void Boss_Update(float deltaTime);
void Boss_Render(HDC hdc);
void Boss_Cleanup(void);

void Boss_TakeDamage(float damage);
bool Boss_IsAlive(void);
RECT Boss_GetRect(void);

// --- 외부 모듈 상호작용 함수 ---

float Boss_GetCenterX(void);
float Boss_GetCenterY(void);
float Boss_GetHPRatio(void);