#include "pattern.h"
#include "pattern_catalog.h"
#include <stdlib.h>

Pattern* Pattern_Create(PatternType type, float duration)
{
    // type에 알맞은 패턴생성함수 포인터를 불러옴
    PatternCreateFunc createFunc = PatternCatalog_GetCreateFunc(type);

    // NULL이 아니라면, 실행
    if (createFunc)
    {
        return createFunc(duration);
    }

    return NULL;
}

void Pattern_Destroy(Pattern* self)
{
    if (self)
    {
        free(self->state); // 상태 데이터 메모리 해제
        free(self);        // 패턴 객체 자체 메모리 해제
    }
}