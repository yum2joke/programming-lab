#include "boss.h"
#include "config.h"
#include <stdbool.h>

static float s_bossX, s_bossY;
static float s_currentHP, s_maxHP;
static RECT s_clientRect;
static bool s_isAlive;

void Boss_Init(RECT clientRect)
{
    s_clientRect = clientRect;
    s_bossX = (float)(clientRect.right - BOSS_WIDTH) / 2.0f;
    s_bossY = 50.0f;
    s_maxHP = BOSS_MAX_HP;
    s_currentHP = BOSS_MAX_HP;
    s_isAlive = true;
}

void Boss_Update(float deltaTime)
{
    if (!s_isAlive)
    {
        return;
    }

    // TODO: 대미지 입는 애니메이션?
}

void Boss_Render(HDC hdc)
{
    if (!s_isAlive)
    {
        return;
    }

    // 보스 본체 그리기
    HBRUSH hBossBrush = CreateSolidBrush(BOSS_COLOR);
    RECT bossRect = { (int)s_bossX, (int)s_bossY, (int)s_bossX + BOSS_WIDTH, (int)s_bossY + BOSS_HEIGHT };
    FillRect(hdc, &bossRect, hBossBrush);
    DeleteObject(hBossBrush);

    // 체력바 회색배경 그리기
    RECT hpBgRect = { 10, 10, s_clientRect.right - 10, 10 + BOSS_HP_BAR_HEIGHT };
    HBRUSH hBgBrush = CreateSolidBrush(BOSS_HP_BAR_BG_COLOR);
    FillRect(hdc, &hpBgRect, hBgBrush);
    DeleteObject(hBgBrush);

    // 현재 체력 그리기
    float hpRatio = s_currentHP / s_maxHP;
    int hpBarWidth = (int)((s_clientRect.right - 20) * hpRatio);
    RECT hpRect = { 10, 10, 10 + hpBarWidth, 10 + BOSS_HP_BAR_HEIGHT };
    HBRUSH hHpBrush = CreateSolidBrush(BOSS_HP_BAR_COLOR);
    FillRect(hdc, &hpRect, hHpBrush);
    DeleteObject(hHpBrush);
}

void Boss_TakeDamage(float damage)
{
    if (!s_isAlive)
    {
        return;
    }

    s_currentHP -= damage;
    if (s_currentHP <= 0)
    {
        s_currentHP = 0;
        s_isAlive = false;
    }
}

bool Boss_IsAlive(void)
{
    return s_isAlive;
}

RECT Boss_GetRect(void)
{
    RECT rect = { (LONG)s_bossX, (LONG)s_bossY, (LONG)(s_bossX + BOSS_WIDTH), (LONG)(s_bossY + BOSS_HEIGHT) };
    return rect;
}