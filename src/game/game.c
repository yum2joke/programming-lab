#include "game.h"

#include "config.h"
#include "collision/collision_manager.h"
#include "entities/actors/actor_manager.h"
#include "entities/actors/boss/boss.h"
#include "entities/actors/player/player.h"
#include "entities/projectiles/projectile.h"
#include "entities/beams/beam.h"
#include "entities/star/star.h"
#include "ui/ui_manager.h"

#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

// 게임 상태 변수
static RECT g_clientRect;
static int g_mouseX, g_mouseY;
static GameState g_gameState = GAME_STATE_MAIN_MENU;

// 게임 초기 상태로 리셋
static void Game_Reset(void)
{
    ActorManager_Init();
    Player_Init(g_clientRect);
    Boss_Spawn(BOSS_TYPE_A, g_clientRect);
    Projectile_Init();
    Beam_Init();
    Star_Init(g_clientRect);
    g_gameState = GAME_STATE_PLAYING;

    ShowCursor(FALSE);
}

// 게임 초기화
void Game_Init(HWND hwnd)
{
    GetClientRect(hwnd, &g_clientRect);
    srand((unsigned int)time(NULL));

    Star_Init(g_clientRect);
    g_gameState = GAME_STATE_MAIN_MENU;
}

// 매 프레임 게임 상태 업데이트: 모든 모듈의 업데이트 함수 호출
void Game_Update(float deltaTime)
{
    if (g_gameState != GAME_STATE_PLAYING)
    {
        return; // 플레이 상태가 아닐 때는 업데이트 중단
    }

    Star_Update(deltaTime, g_clientRect);
    Player_Update(deltaTime, g_mouseX, g_mouseY);
    Projectile_Update(deltaTime, g_clientRect);
    Beam_Update(deltaTime, g_clientRect);
    Boss_Update(deltaTime);

    // 충돌 처리
    CollisionManager_CheckAll();

    if (!Player_IsAlive())
    {
        Game_SetGameOver();
    }
    else if (!Boss_IsAlive())
    {
        Game_SetGameClear();
    }
}

// 화면 렌더링: 모든 모듈의 렌더링 함수 호출
void Game_Render(HWND hwnd)
{
    HDC hdc = GetDC(hwnd);
    
    // 더블 버퍼링을 위한 메모리 DC 및 비트맵 생성
    HDC hMemDC = CreateCompatibleDC(hdc);
    HBITMAP hBitmap = CreateCompatibleBitmap(hdc, g_clientRect.right, g_clientRect.bottom);
    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);

    // 배경 그리기
    FillRect(hMemDC, &g_clientRect, (HBRUSH)GetStockObject(BLACK_BRUSH));
    Star_Render(hMemDC);

    // 게임 요소 그리기
    if (g_gameState != GAME_STATE_MAIN_MENU)
    {
        Boss_Render(hMemDC);
        Projectile_Render(hMemDC);
        Beam_Render(hMemDC);
        Player_Render(hMemDC);
    }
    
    // UI 그리기
    UIManager_Render(hMemDC, g_clientRect, g_gameState);

    // 백 버퍼의 내용을 화면으로 복사
    BitBlt(hdc, 0, 0, g_clientRect.right, g_clientRect.bottom, hMemDC, 0, 0, SRCCOPY);

    // 사용한 GDI 리소스 정리
    SelectObject(hMemDC, hOldBitmap);
    DeleteObject(hBitmap);
    DeleteDC(hMemDC);
    ReleaseDC(hwnd, hdc);
}

// 창 비활성화 시 처리
void Game_HandleActivate(WPARAM wParam)
{
    if (LOWORD(wParam) == WA_INACTIVE)
    {
        // 플레이 중일 때만 비활성화 시 일시정지로 전환
        if (g_gameState == GAME_STATE_PLAYING)
        {
            g_gameState = GAME_STATE_PAUSED;
            ShowCursor(TRUE);
            ClipCursor(NULL);
        }
    }
}

// 키보드 입력 처리
void Game_HandleKeyDown(HWND hwnd, WPARAM wParam)
{
    if (wParam == VK_ESCAPE)
    {
        // 게임 플레이 중에만 일시정지 가능
        if (g_gameState == GAME_STATE_PLAYING)
        {
            g_gameState = GAME_STATE_PAUSED;
            ShowCursor(TRUE);
            ClipCursor(NULL);
        }
        // 일시정지 해제
        else if (g_gameState == GAME_STATE_PAUSED)
        {
            g_gameState = GAME_STATE_PLAYING;
            ShowCursor(FALSE);
            RECT clipRect;
            GetClientRect(hwnd, &clipRect);
            ClientToScreen(hwnd, (LPPOINT)&clipRect.left);
            ClientToScreen(hwnd, (LPPOINT)&clipRect.right);
            ClipCursor(&clipRect);
        }
    }
}

// 마우스 이동 처리
void Game_HandleMouseMove(LPARAM lParam)
{
    g_mouseX = LOWORD(lParam);
    g_mouseY = HIWORD(lParam);
}

// 마우스 클릭 처리
// TODO: down이 아닌 up으로 구현. hover랑 같이 구현필요
void Game_HandleMouseDown(LPARAM lParam)
{
    int mouseX = LOWORD(lParam);
    int mouseY = HIWORD(lParam);

    UIAction action = UIManager_CheckButtonClick(mouseX, mouseY, g_clientRect, g_gameState);
    switch (action)
    {
        case UI_ACTION_START:
        case UI_ACTION_RESTART:
            Game_Reset();
            break;
        case UI_ACTION_MAIN_MENU:
            g_gameState = GAME_STATE_MAIN_MENU;
            break;
        default:
            break;
    }
}

// 창 크기 변경 처리
void Game_HandleSize(HWND hwnd)
{
    GetClientRect(hwnd, &g_clientRect);
    if (g_gameState == GAME_STATE_PLAYING)
    {
        RECT clipRect;
        GetClientRect(hwnd, &clipRect);
        ClientToScreen(hwnd, (LPPOINT)&clipRect.left);
        ClientToScreen(hwnd, (LPPOINT)&clipRect.right);
        ClipCursor(&clipRect);
    }
}

// 게임 종료 시 리소스 정리
void Game_Cleanup(void)
{
    Boss_Cleanup();
    ClipCursor(NULL);
    ShowCursor(TRUE);
}

void Game_SetGameOver(void)
{
    g_gameState = GAME_STATE_GAME_OVER;
    ShowCursor(TRUE);
    ClipCursor(NULL);
}

void Game_SetGameClear(void)
{
    g_gameState = GAME_STATE_GAME_CLEAR;
    ShowCursor(TRUE);
    ClipCursor(NULL);
}