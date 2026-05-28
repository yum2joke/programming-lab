#pragma once

#include <windows.h>

typedef enum
{
    GAME_STATE_MAIN_MENU,
    GAME_STATE_PLAYING,
    GAME_STATE_PAUSED,
    GAME_STATE_GAME_OVER,
    GAME_STATE_GAME_CLEAR
} GameState;

// 게임 초기화
void Game_Init(HWND hwnd);

// 매 프레임 업데이트 및 렌더링
void Game_Update(float deltaTime);
void Game_Render(HWND hwnd);

void Game_Cleanup(void);

// 윈도우 메시지 처리
void Game_HandleActivate(WPARAM wParam);
void Game_HandleKeyDown(HWND hwnd, WPARAM wParam);
void Game_HandleMouseMove(LPARAM lParam);
void Game_HandleMouseDown(LPARAM lParam);
void Game_HandleSize(HWND hwnd);

// 게임 상태 변경
void Game_SetGameOver(void);
void Game_SetGameClear(void);