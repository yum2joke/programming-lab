#pragma once

#include <windows.h>

void Player_Init(RECT clientRect);
void Player_Update(float deltaTime, int mouseX, int mouseY);
void Player_Render(HDC hdc);

// static으로 보호된 (멤버)변수 접근하는 함수
float Player_GetX(void);
float Player_GetY(void);
float Player_GetCenterX(void);
float Player_GetCenterY(void);