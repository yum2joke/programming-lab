#include "boss.h"

#include "boss_catalog.h"
#include "config.h"
#include "game/patterns/pattern.h"

#include <stdbool.h>
#include <stdlib.h>

typedef struct {
    float x;
    float y;
    float currentHP;
    float maxHP;
    RECT clientRect;
    bool isAlive;

    // --- 보스 프레임워크 상태 변수 ---
    Pattern* activePatterns[MAX_CONCURRENT_PATTERNS]; // 동시에 실행 중인 패턴들
    const BossDesc* currentBossDesc;      // 보스 설계도
    int currentPhaseIndex;                // 페이즈 인덱스
    int lastActionIndex;                  // 마지막으로 선택한 행동(PhasePattern) 인덱스
} Boss;

static Boss s_boss;

void Boss_Spawn(BossType type, RECT clientRect)
{
    // 이전 보스 데이터 정리
    Boss_Cleanup();

    // 설계도 불러오기
    s_boss.currentBossDesc = BossCatalog_GetDesc(type);
    if (!s_boss.currentBossDesc)
    {
        return;
    }

    s_boss.clientRect = clientRect;
    s_boss.x = (float)(clientRect.right - BOSS_WIDTH) / 2.0f;
    s_boss.y = 50.0f;
    s_boss.maxHP = s_boss.currentBossDesc->maxHp;
    s_boss.currentHP = s_boss.maxHP;
    s_boss.isAlive = true;
    s_boss.currentPhaseIndex = -1; // 페이즈 미설정 상태
    s_boss.lastActionIndex = -1;

    for (int i = 0; i < MAX_CONCURRENT_PATTERNS; i++)
    {
        s_boss.activePatterns[i] = NULL;
    }
}

void Boss_Update(float deltaTime)
{
    if (!s_boss.isAlive || !s_boss.currentBossDesc)
    {
        return;
    }

    float hpRatio = Boss_GetHPRatio();  // 현재 보스의 체력
    int newPhaseIndex = -1;
    bool phaseChanged = false;          // 페이즈가 변경되었는가?
    bool patternFinished = false;       // 패턴이 시간이 지나 끝났는가?

    // 1. 현재 HP에 알맞은 페이즈 찾기

    for (int i = 0; i < s_boss.currentBossDesc->phaseCount; ++i)
    {
        if (hpRatio <= s_boss.currentBossDesc->phases[i].hpThreshold)
        {
            newPhaseIndex = i;
        }
    }
    // 페이즈 변경되었는지 여부 판단
    phaseChanged = (newPhaseIndex != -1 && newPhaseIndex != s_boss.currentPhaseIndex);

    // 2. 현재 패턴들 실행 및 모두 종료되었는지 확인
    bool hasActive = false;
    bool allFinished = true;

    for (int i = 0; i < MAX_CONCURRENT_PATTERNS; i++)
    {
        if (s_boss.activePatterns[i])
        {
            hasActive = true;
            PatternStatus status = s_boss.activePatterns[i]->update(s_boss.activePatterns[i], deltaTime, Boss_GetCenterX(), Boss_GetCenterY());

            // 패턴이 끝나면 즉시 메모리 해제 및 NULL 처리
            if (status == PATTERN_FINISHED)
            {
                s_boss.activePatterns[i]->destroy(s_boss.activePatterns[i]);
                s_boss.activePatterns[i] = NULL;
            }
            else
            {
                allFinished = false; // 하나라도 돌고 있으면 끝나지 않음
            }
        }
    }

    if (!hasActive || allFinished)
    {
        patternFinished = true;
    }

    // 3. 페이즈가 변경되거나 or 패턴이 시간이 다 되어 끝났을 시- 새로운 패턴으로 교체

    if (phaseChanged || patternFinished)
    {
        // 페이즈 변경 시- 인덱스 갱신 및 lastPatternType 초기화
        if (phaseChanged)
        {
            s_boss.currentPhaseIndex = newPhaseIndex;
            s_boss.lastActionIndex = -1;
        }

        // 3-1. 마지막 패턴을 제외한 전체 가중치 계산

        const BossPhase* currentPhase = &s_boss.currentBossDesc->phases[s_boss.currentPhaseIndex];
        float totalWeight = 0;
        bool canExclude = (currentPhase->patternArrCount > 1);
        
        for (int i = 0; i < currentPhase->patternArrCount; ++i)
        {
            // 방금 사용했던 마지막 패턴은 제외
            // 패턴이 하나밖에 없으면, 재사용해야 하니 패스
            if (canExclude && i == s_boss.lastActionIndex) continue;

            // 마지막 패턴 제외 시, 총합이 1.0이 아니게 되므로 totalWeight 계산필요
            totalWeight += currentPhase->patternArr[i].weight;
        }

        // 3-2. 패턴 선택

        const PhasePattern* selectedPattern = NULL;     // 새로 선택한 패턴. 기본은 NULL
        int newActionIndex = -1;
        float randomValue = (float)rand() / RAND_MAX * totalWeight; // 0 ~ totalWeight 사이 랜덤값

        for (int i = 0; i < currentPhase->patternArrCount; ++i)
        {
            // 방금 사용했던 마지막 패턴은 제외
            if (canExclude && i == s_boss.lastActionIndex) continue;

            // 가중치 랜덤 알고리즘
            // 랜덤값이 가중치보다 작을 때 선택
            if (randomValue < currentPhase->patternArr[i].weight)
            {
                selectedPattern = &currentPhase->patternArr[i];
                newActionIndex = i;
                break;
            }
            // 랜덤값 -= 가중치
            randomValue -= currentPhase->patternArr[i].weight;
        }

        // 3-3. 패턴 교체

        // 패턴 새로 선택시-
        if (selectedPattern)
        {
            // 페이즈 강제 변경 시 아직 돌고 있는 기존 패턴들 싹 다 삭제
            for (int i = 0; i < MAX_CONCURRENT_PATTERNS; i++)
            {
                if (s_boss.activePatterns[i])
                {
                    s_boss.activePatterns[i]->destroy(s_boss.activePatterns[i]);
                    s_boss.activePatterns[i] = NULL;
                }
            }

            // 다중 패턴 생성 및 주입
            for (int i = 0; i < selectedPattern->patternCount; i++)
            {
                s_boss.activePatterns[i] = Pattern_Create(selectedPattern->patterns[i].type, &selectedPattern->patterns[i].desc);
            }
            s_boss.lastActionIndex = newActionIndex;
        }
    }
}

void Boss_Cleanup(void)
{
    for (int i = 0; i < MAX_CONCURRENT_PATTERNS; i++)
    {
        if (s_boss.activePatterns[i])
        {
            s_boss.activePatterns[i]->destroy(s_boss.activePatterns[i]);
            s_boss.activePatterns[i] = NULL;
        }
    }
}

void Boss_Render(HDC hdc)
{
    if (!s_boss.isAlive)
    {
        return;
    }

    // 보스 본체 그리기
    HBRUSH hBossBrush = CreateSolidBrush(BOSS_COLOR);
    RECT bossRect = { (int)s_boss.x, (int)s_boss.y, (int)s_boss.x + BOSS_WIDTH, (int)s_boss.y + BOSS_HEIGHT };
    FillRect(hdc, &bossRect, hBossBrush);
    DeleteObject(hBossBrush);

    // 체력바 회색배경 그리기
    RECT hpBgRect = { 10, 10, s_boss.clientRect.right - 10, 10 + BOSS_HP_BAR_HEIGHT };
    HBRUSH hBgBrush = CreateSolidBrush(BOSS_HP_BAR_BG_COLOR);
    FillRect(hdc, &hpBgRect, hBgBrush);
    DeleteObject(hBgBrush);

    // 현재 체력 그리기
    float hpRatio = s_boss.currentHP / s_boss.maxHP;
    int hpBarWidth = (int)((s_boss.clientRect.right - 20) * hpRatio);
    RECT hpRect = { 10, 10, 10 + hpBarWidth, 10 + BOSS_HP_BAR_HEIGHT };
    HBRUSH hHpBrush = CreateSolidBrush(BOSS_HP_BAR_COLOR);
    FillRect(hdc, &hpRect, hHpBrush);
    DeleteObject(hHpBrush);
}

void Boss_TakeDamage(float damage)
{
    if (!s_boss.isAlive)
    {
        return;
    }

    s_boss.currentHP -= damage;
    if (s_boss.currentHP <= 0)
    {
        s_boss.currentHP = 0;
        s_boss.isAlive = false;
    }
}

float Boss_GetHPRatio(void)
{
    return (s_boss.maxHP > 0) ? (s_boss.currentHP / s_boss.maxHP) : 0.0f;
}

bool Boss_IsAlive(void)
{
    return s_boss.isAlive;
}

RECT Boss_GetRect(void)
{
    RECT rect = { (LONG)s_boss.x, (LONG)s_boss.y, (LONG)(s_boss.x + BOSS_WIDTH), (LONG)(s_boss.y + BOSS_HEIGHT) };
    return rect;
}

float Boss_GetCenterX(void)
{
    return s_boss.x + BOSS_WIDTH / 2.0f;
}

float Boss_GetCenterY(void)
{
    return s_boss.y + BOSS_HEIGHT / 2.0f;
}
