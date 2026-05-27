#pragma once

#include "beam_types.h"

#include <windows.h>

// --- 빔 전체관리자 함수 ---
void Beam_Init(void);
void Beam_Update(float deltaTime, RECT clientRect);
void Beam_Render(HDC hdc);

void Beam_Spawn(int ownerId, AttackEntityType type, float startX, float startY, float dirX, float dirY);

// --- 외부 모듈 상호작용 함수 ---
int Beam_GetPoolSize(void);
const Beam* Beam_GetFromPool(int index);