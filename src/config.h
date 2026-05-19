#pragma once

#include <windows.h>
#include <tchar.h>

// TODO: 개발 완료 후, 상수들 type 알잘딱하기 (가변변수들은 내가 개발하면서 const 없애지않을까)

// 게임 설정 상수
const int PLAYER_SIZE = 20;
const float PLAYER_SPEED = 500.0f; // 초당 픽셀 단위 속도
const COLORREF PLAYER_COLOR = RGB(150, 150, 255);

// 조준선
const int CROSSHAIR_CIRCLE_RADIUS = 5;
const COLORREF CROSSHAIR_LINE_COLOR = RGB(50, 50, 0);
const int CROSSHAIR_LINE_THICKNESS = 3;

// 일시정지 UI
const COLORREF PAUSE_TEXT_COLOR = RGB(255, 255, 255);
const int PAUSE_FONT_SIZE = 48;
const TCHAR* PAUSE_FONT_NAME = _T("Arial");
const TCHAR* PAUSE_TEXT = _T("일시정지");

// 배경 별 효과
#define STAR_COUNT 100
float STAR_SPEED = 700.0f;
const COLORREF STAR_COLOR = RGB(150, 150, 150);
const int STAR_MAX_SIZE = 5;