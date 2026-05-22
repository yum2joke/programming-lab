#pragma once

#include <windows.h>
#include "beam_types.h"

// --- 빔 전체관리자 함수 ---
void Beam_Init(void);
void Beam_Update(float deltaTime, RECT clientRect);
void Beam_Render(HDC hdc);