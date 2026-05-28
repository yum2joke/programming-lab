#pragma once

#include <windows.h>
#include <stdbool.h>
#include "game/game.h"

// 버튼 액션 정의
typedef enum {
    UI_ACTION_NONE,
    UI_ACTION_START,
    UI_ACTION_RESTART,
    UI_ACTION_MAIN_MENU
} UIAction;

// 화면(Screen) UI 렌더링 위임 함수
void UIManager_Render(HDC hdc, RECT clientRect, GameState gameState);

// 버튼 클릭 영역 계산 및 확인 함수
UIAction UIManager_CheckButtonClick(int mouseX, int mouseY, RECT clientRect, GameState gameState);