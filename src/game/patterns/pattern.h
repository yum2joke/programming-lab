#pragma once

#include "pattern_types.h"

Pattern* Pattern_Create(PatternType type, float duration);
void Pattern_Destroy(Pattern* self);