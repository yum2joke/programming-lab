#include <stdbool.h>
#include <math.h>
#include <time.h>

#include "game.h"
#include "config.h"

#include "entities/player/player.h"
#include "entities/bullet/bullet.h"
#include "entities/boss/boss.h"
#include "entities/star/star.h"
#include "collision/collision_manager.h"

// 게임 상태 변수
static RECT g_clientRect;
static int g_mouseX, g_mouseY;
static bool g_isPaused = false;
static float g_fireCooldown = 0.0f;
static bool g_isGameOver = false;

// 게임 초기화
void Game_Init(HWND hwnd)
{
    GetClientRect(hwnd, &g_clientRect);
    ShowCursor(FALSE); // 시스템 커서 숨기기

    // 시드 설정
    srand((unsigned int)time(NULL));

    Player_Init(g_clientRect);
    Bullet_Init();
    Boss_Init(g_clientRect);
    Star_Init(g_clientRect);
    g_isGameOver = false;
}

// 매 프레임 게임 상태 업데이트: 모든 모듈의 업데이트 함수 호출
void Game_Update(float deltaTime)
{
    if (g_isPaused || g_isGameOver)
    {
        return; // 일시정지 상태에서는 업데이트 중단
    }

    // 자동 발사 로직
    g_fireCooldown -= deltaTime;
    if (g_fireCooldown <= 0.0f)
    {
        g_fireCooldown = FIRE_RATE; // 쿨다운 초기화

        float playerCenterX = Player_GetCenterX();
        float playerCenterY = Player_GetCenterY();
        float dirX = g_mouseX - playerCenterX;
        float dirY = g_mouseY - playerCenterY;
        float length = sqrtf(dirX * dirX + dirY * dirY);

        if (length > 0.001f)
        {
            Bullet_Fire(LAYER_PLAYER_BULLET, playerCenterX, playerCenterY, dirX / length, dirY / length);
        }
        else    // 마우스가 플레이어 위에 있을 경우, 위로 발사
        {
            Bullet_Fire(LAYER_PLAYER_BULLET, playerCenterX, playerCenterY, 0.0f, -1.0f);
        }
    }

    Star_Update(deltaTime, g_clientRect);
    Player_Update(deltaTime);
    Bullet_Update(deltaTime, g_clientRect);
    Boss_Update(deltaTime);

    // 충돌 처리
    CollisionManager_CheckAll();
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

    // 조준선 그리기
    if (!g_isPaused)
    {
        HPEN hLinePen = CreatePen(PS_SOLID, CROSSHAIR_LINE_THICKNESS, CROSSHAIR_LINE_COLOR);
        HPEN hOldPen = (HPEN)SelectObject(hMemDC, hLinePen);

        float playerCenterX = Player_GetCenterX();
        float playerCenterY = Player_GetCenterY();
        MoveToEx(hMemDC, (int)playerCenterX, (int)playerCenterY, NULL);

        // 플레이어 중심에서 마우스 커서를 관통하는 직선 계산
        float dirX = g_mouseX - playerCenterX;
        float dirY = g_mouseY - playerCenterY;
        float length = sqrtf(dirX * dirX + dirY * dirY);    // 길이 구하기

        if (length > 0.001f)
        {
            // 벡터 정규화로 화면 밖 길게 그려버리기
            float endX = playerCenterX + (dirX / length) * 2000.0f;
            float endY = playerCenterY + (dirY / length) * 2000.0f;
            LineTo(hMemDC, (int)endX, (int)endY);
        }

        Ellipse(hMemDC, g_mouseX - CROSSHAIR_CIRCLE_RADIUS, g_mouseY - CROSSHAIR_CIRCLE_RADIUS, g_mouseX + CROSSHAIR_CIRCLE_RADIUS, g_mouseY + CROSSHAIR_CIRCLE_RADIUS);

        SelectObject(hMemDC, hOldPen);
        DeleteObject(hLinePen);
    }

    // 게임 요소 그리기
    Boss_Render(hMemDC);
    Bullet_Render(hMemDC);
    Player_Render(hMemDC);
    
    // 게임오버 UI 그리기
    if (g_isGameOver)
    {
        SetTextColor(hMemDC, GAMEOVER_TEXT_COLOR);
        SetBkMode(hMemDC, TRANSPARENT);
        HFONT hFont = CreateFont(GAMEOVER_FONT_SIZE, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, PAUSE_FONT_NAME);
        HFONT hOldFont = (HFONT)SelectObject(hMemDC, hFont);

        DrawText(hMemDC, GAMEOVER_TEXT, -1, &g_clientRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        SelectObject(hMemDC, hOldFont);
        DeleteObject(hFont);
    }

    // 일시정지 UI 그리기
    if (g_isPaused)
    {
        SetTextColor(hMemDC, PAUSE_TEXT_COLOR);
        SetBkMode(hMemDC, TRANSPARENT);
        HFONT hFont = CreateFont(PAUSE_FONT_SIZE, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, PAUSE_FONT_NAME);
        HFONT hOldFont = (HFONT)SelectObject(hMemDC, hFont);

        DrawText(hMemDC, PAUSE_TEXT, -1, &g_clientRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        SelectObject(hMemDC, hOldFont);
        DeleteObject(hFont);
    }

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
        if (!g_isPaused)
        {
            g_isPaused = true;
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
        g_isPaused = !g_isPaused;
        if (g_isPaused)
        {
            ShowCursor(TRUE);
            ClipCursor(NULL);
        }
        else
        {
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

// 창 크기 변경 처리
void Game_HandleSize(HWND hwnd)
{
    GetClientRect(hwnd, &g_clientRect);
    if (!g_isPaused)
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
    ClipCursor(NULL);
    ShowCursor(TRUE);
}

void Game_SetGameOver(void)
{
    g_isGameOver = true;
}