#pragma once

#include "beam_types.h"

#include <windows.h>

// --- 빔 전체관리자 함수 ---
void Beam_Init(void);
void Beam_Update(float deltaTime, RECT clientRect);
void Beam_Render(HDC hdc);