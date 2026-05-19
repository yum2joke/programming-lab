#pragma once

#include <windows.h>

void Star_Init(RECT clientRect);
void Star_Update(float deltaTime, RECT clientRect);
void Star_Render(HDC hdc);