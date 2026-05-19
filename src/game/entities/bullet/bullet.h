#pragma once

#include <windows.h>

void Bullet_Init(void);
void Bullet_Update(float deltaTime, RECT clientRect);
void Bullet_Render(HDC hdc);

// 총알 발사 요청 함수
void Bullet_Fire(float startX, float startY, float dirX, float dirY);