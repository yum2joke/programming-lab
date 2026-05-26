#include "pattern_catalog.h"
#include "rotating/rotating.h"
#include "aimed/aimed.h"

// type - 패턴생성함수 포인터를 매핑한 배열
static const PatternCreateFunc s_pattern_catalog[PATTERN_TYPE_COUNT] = {
    [PATTERN_ROTATING] = RotatingPattern_Create,
    [PATTERN_AIMED] = AimedPattern_Create,
};

PatternCreateFunc PatternCatalog_GetCreateFunc(PatternType type)
{
    // 유효하지 않은 type 예외처리
    if (type < 0 || type >= PATTERN_TYPE_COUNT)
    {
        return NULL;
    }

    // 패턴생성함수 포인터 리턴
    return s_pattern_catalog[type];
}