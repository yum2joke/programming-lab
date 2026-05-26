#pragma once

#include "game/attacks/attack_types.h"

// --- 개별 공격 형태 함수 라이브러리 ---

// 단일 투사체 발사
void Attack_SingleShot(float originX, float originY, float dirX, float dirY, AttackEntityType attackType);

// bulletCount만큼 투사체 원형 발사
void Attack_CircularBurst(float originX, float originY, int bulletCount, float startAngle, AttackEntityType attackType);

// AttackDesc에 맞는 공격 실행
void Attack_Execute(float originX, float originY, float centerAngle, const AttackDesc* desc);