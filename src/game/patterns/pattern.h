#pragma once

#include "pattern_types.h"

Pattern* Pattern_Create(PatternType type, const PatternDesc* desc);
void Pattern_Destroy(Pattern* self);

// 플레이어 조준 유틸리티
float Pattern_GetAngleToPlayer(float originX, float originY);