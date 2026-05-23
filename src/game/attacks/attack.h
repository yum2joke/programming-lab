#pragma once

#include <windows.h>

// --- 개별 공격 형태 함수 라이브러리 ---

// 단일 투사체 발사
void Attack_SingleShot(float originX, float originY, float dirX, float dirY, void (*createFunc)(float, float, float, float));

// bulletCount만큼 투사체 발사
void Attack_CircularBurst(float originX, float originY, int bulletCount, float startAngle, void (*createFunc)(float, float, float, float));