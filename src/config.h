#pragma once

#include <windows.h>
#include <tchar.h>

// 게임 설정 상수
#define PLAYER_SIZE 20
#define PLAYER_SPEED 500.0f // 초당 픽셀 단위 속도
#define PLAYER_COLOR RGB(150, 150, 255)

// 조준선
#define CROSSHAIR_CIRCLE_RADIUS 5
#define CROSSHAIR_LINE_COLOR RGB(50, 50, 0)
#define CROSSHAIR_LINE_THICKNESS 3

// 일시정지 UI
#define PAUSE_TEXT_COLOR RGB(255, 255, 255)
#define PAUSE_FONT_SIZE 48
#define PAUSE_FONT_NAME _T("Arial")
#define PAUSE_TEXT _T("일시정지")

// 배경 별 효과
#define STAR_COUNT 100
#define STAR_SPEED 700.0f
#define STAR_COLOR RGB(150, 150, 150)
#define STAR_MAX_SIZE 5

// 총알
#define BULLET_MAX_COUNT 50
#define BULLET_SPEED 1500.0f
#define BULLET_SIZE 10
#define BULLET_COLOR RGB(181, 230, 29)
#define FIRE_RATE 0.05f // 발당 대기 시간
#define BULLET_DAMAGE 10.0f

// 보스
#define BOSS_WIDTH 200
#define BOSS_HEIGHT 200
#define BOSS_MAX_HP 1000.0f
#define BOSS_COLOR RGB(100, 100, 100)
#define BOSS_HP_BAR_COLOR RGB(255, 0, 0)
#define BOSS_HP_BAR_BG_COLOR RGB(50, 50, 50)
#define BOSS_HP_BAR_HEIGHT 10