#include "boss_catalog.h"

#include "boss_A/boss_A.h"
// #include "boss_B/boss_B.h"

// 모든 보스 설계도가 담긴 배열
static const BossDesc* s_boss_catalog[BOSS_TYPE_COUNT] = {
    [BOSS_TYPE_A] = &BossA_Desc,
    // [BOSS_TYPE_B] = &BossB_Desc,
};

const BossDesc* BossCatalog_GetDesc(BossType type)
{
    if (type < 0 || type >= BOSS_TYPE_COUNT)
    {
        return NULL;
    }

    return s_boss_catalog[type];
}