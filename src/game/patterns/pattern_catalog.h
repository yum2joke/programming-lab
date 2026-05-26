#pragma once

#include "pattern_types.h"

// 패턴생성함수 포인터 타입
typedef Pattern* (*PatternCreateFunc)(float duration);

// type에 알맞은 패턴생성함수 포인터를 리턴
PatternCreateFunc PatternCatalog_GetCreateFunc(PatternType type);