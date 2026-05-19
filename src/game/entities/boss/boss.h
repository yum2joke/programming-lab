#pragma once

#include <windows.h>
#include <stdbool.h>

void Boss_Init(RECT clientRect);
void Boss_Update(float deltaTime);
void Boss_Render(HDC hdc);

void Boss_TakeDamage(float damage);
bool Boss_IsAlive(void);
RECT Boss_GetRect(void);