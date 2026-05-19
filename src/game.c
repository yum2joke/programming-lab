#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#include "game.h"
#include "config.h"

// 게임 상태 변수
static float g_playerX, g_playerY;
static RECT g_clientRect;
static int g_mouseX, g_mouseY;
static bool g_isPaused = false;

// 배경 별 효과를 위한 구조체 및 배열
typedef struct {
    float x, y;
    int size;
} Star;
static Star g_stars[STAR_COUNT];

// 총알을 위한 구조체 및 배열
typedef struct {
    float x, y;
    float dx, dy; // 방향 벡터
    bool active;
} Bullet;
static Bullet g_bullets[BULLET_MAX_COUNT];

static float g_fireCooldown = 0.0f;

// 게임 초기화
void Game_Init(HWND hwnd)
{
    GetClientRect(hwnd, &g_clientRect);
    g_playerX = (float)(g_clientRect.right - PLAYER_SIZE) / 2.0f;
    g_playerY = (float)(g_clientRect.bottom - PLAYER_SIZE) / 2.0f;
    ShowCursor(FALSE); // 시스템 커서 숨기기

    // 배경 별 초기화
    srand((unsigned int)time(NULL)); // 난수 시드 설정
    for (int i = 0; i < STAR_COUNT; ++i)
    {
        g_stars[i].x = (float)(rand() % g_clientRect.right);
        g_stars[i].y = (float)(rand() % g_clientRect.bottom);
        g_stars[i].size = (rand() % STAR_MAX_SIZE) + 1;
    }

    // 총알 배열 초기화
    for (int i = 0; i < BULLET_MAX_COUNT; ++i)
    {
        g_bullets[i].active = false;
    }
}

// 매 프레임 게임 상태를 업데이트
void Game_Update(float deltaTime)
{
    if (g_isPaused)
    {
        return; // 일시정지 상태에서는 업데이트 중단
    }

    // 배경 별 위치 업데이트
    for (int i = 0; i < STAR_COUNT; ++i)
    {
        g_stars[i].y -= STAR_SPEED * deltaTime; // 별을 위로 이동
        // 별이 화면 상단을 벗어나면, 화면 하단에서 다시 생성
        if (g_stars[i].y < 0)
        {
            g_stars[i].y = (float)g_clientRect.bottom;
            g_stars[i].x = (float)(rand() % g_clientRect.right);
            g_stars[i].size = (rand() % 5) + 1;
        }
    }

    // 발사 쿨다운 감소
    g_fireCooldown -= deltaTime;

    // 자동 발사
    if (g_fireCooldown <= 0.0f)
    {
        g_fireCooldown = FIRE_RATE; // 쿨다운 초기화

        // 비활성화된 총알을 찾아 발사
        for (int i = 0; i < BULLET_MAX_COUNT; ++i)
        {
            if (!g_bullets[i].active)
            {
                g_bullets[i].active = true;

                // 시작 위치: 플레이어 중심
                float playerCenterX = g_playerX + PLAYER_SIZE / 2.0f;
                float playerCenterY = g_playerY + PLAYER_SIZE / 2.0f;
                g_bullets[i].x = playerCenterX;
                g_bullets[i].y = playerCenterY;

                // 방향 벡터 계산 (플레이어 -> 마우스)
                float dirX = g_mouseX - playerCenterX;
                float dirY = g_mouseY - playerCenterY;
                float length = sqrtf(dirX * dirX + dirY * dirY);

                if (length > 0.001f)
                {
                    g_bullets[i].dx = (dirX / length);
                    g_bullets[i].dy = (dirY / length);
                }
                else // 마우스가 플레이어 위에 있을 경우, 위로 발사
                {
                    g_bullets[i].dx = 0.0f;
                    g_bullets[i].dy = -1.0f;
                }
                
                break; // 한 프레임에 한 발만 발사
            }
        }
    }

    // 총알 위치 업데이트
    for (int i = 0; i < BULLET_MAX_COUNT; ++i)
    {
        if (g_bullets[i].active)
        {
            g_bullets[i].x += g_bullets[i].dx * BULLET_SPEED * deltaTime;
            g_bullets[i].y += g_bullets[i].dy * BULLET_SPEED * deltaTime;

            // 화면을 벗어나면 비활성화
            if (g_bullets[i].x < 0 || g_bullets[i].x > g_clientRect.right ||
                g_bullets[i].y < 0 || g_bullets[i].y > g_clientRect.bottom)
            {
                g_bullets[i].active = false;
            }
        }
    }

    // 이번 프레임의 이동량
    float deltaX = 0.0f;
    float deltaY = 0.0f;

    // 기본적으로 위로 자동 스크롤
    deltaY -= PLAYER_SPEED * 0.1f;

    // 키 입력에 따른 이동량 계산
    if (GetAsyncKeyState('A') & 0x8000) deltaX -= PLAYER_SPEED;
    if (GetAsyncKeyState('D') & 0x8000) deltaX += PLAYER_SPEED;
    if (GetAsyncKeyState('W') & 0x8000) deltaY -= PLAYER_SPEED;
    if (GetAsyncKeyState('S') & 0x8000)
    {
        // 화면 하단에 가까워질수록 하단 도망 속도 감소
        float lowerLimitY = (float)g_clientRect.bottom - PLAYER_SIZE;
        if (g_playerY < lowerLimitY)
        {
            float ratio = (lowerLimitY - g_playerY) / lowerLimitY;
            deltaY += PLAYER_SPEED * ratio;
        }
    }

    // 프레임 시간에 맞춰 최종 위치 업데이트
    g_playerX += deltaX * deltaTime;
    g_playerY += deltaY * deltaTime;

    // 경계 처리
    if (g_playerX < g_clientRect.left) g_playerX = g_clientRect.left;
    if (g_playerX + PLAYER_SIZE > g_clientRect.right) g_playerX = (float)g_clientRect.right - PLAYER_SIZE;
    if (g_playerY < g_clientRect.top) g_playerY = g_clientRect.top;
    if (g_playerY + PLAYER_SIZE > g_clientRect.bottom) g_playerY = (float)g_clientRect.bottom - PLAYER_SIZE;
}

// 화면에 게임 요소 렌더링
void Game_Render(HWND hwnd)
{
    HDC hdc = GetDC(hwnd);
    
    // 더블 버퍼링을 위한 메모리 DC 및 비트맵 생성
    HDC hMemDC = CreateCompatibleDC(hdc);
    HBITMAP hBitmap = CreateCompatibleBitmap(hdc, g_clientRect.right, g_clientRect.bottom);
    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);

    // 배경 그리기
    FillRect(hMemDC, &g_clientRect, (HBRUSH)GetStockObject(BLACK_BRUSH));
    
    // 배경 별 그리기
    HBRUSH hStarBrush = CreateSolidBrush(STAR_COLOR);
    for (int i = 0; i < STAR_COUNT; ++i)
    {
        // 별의 크기를 반영하여 사각형으로 그리기
        RECT starRect = {
            (int)g_stars[i].x,
            (int)g_stars[i].y,
            (int)g_stars[i].x + g_stars[i].size,
            (int)g_stars[i].y + g_stars[i].size
        };
        FillRect(hMemDC, &starRect, hStarBrush);
    }
    DeleteObject(hStarBrush);

    // 총알 그리기
    HBRUSH hBulletBrush = CreateSolidBrush(BULLET_COLOR);
    for (int i = 0; i < BULLET_MAX_COUNT; ++i)
    {
        if (g_bullets[i].active)
        {
            RECT bulletRect = {
                (int)g_bullets[i].x - BULLET_SIZE / 2,
                (int)g_bullets[i].y - BULLET_SIZE / 2,
                (int)g_bullets[i].x + BULLET_SIZE / 2,
                (int)g_bullets[i].y + BULLET_SIZE / 2
            };
            FillRect(hMemDC, &bulletRect, hBulletBrush);
        }
    }
    DeleteObject(hBulletBrush);
    
    // 조준선 그리기 (일시정지에는 X)
    if (!g_isPaused)
    {
        HPEN hLinePen = CreatePen(PS_SOLID, CROSSHAIR_LINE_THICKNESS, CROSSHAIR_LINE_COLOR);
        HPEN hOldPen = (HPEN)SelectObject(hMemDC, hLinePen);

        float playerCenterX = g_playerX + PLAYER_SIZE / 2.0f;
        float playerCenterY = g_playerY + PLAYER_SIZE / 2.0f;
        MoveToEx(hMemDC, (int)playerCenterX, (int)playerCenterY, NULL);

        // 플레이어 중심에서 마우스 커서를 관통하는 직선 계산
        float dirX = g_mouseX - playerCenterX;
        float dirY = g_mouseY - playerCenterY;
        float length = sqrtf(dirX * dirX + dirY * dirY);

        if (length > 0.001f)
        {
            float endX = playerCenterX + (dirX / length) * 2000.0f;
            float endY = playerCenterY + (dirY / length) * 2000.0f;
            LineTo(hMemDC, (int)endX, (int)endY);
        }

        Ellipse(hMemDC, g_mouseX - CROSSHAIR_CIRCLE_RADIUS, g_mouseY - CROSSHAIR_CIRCLE_RADIUS, g_mouseX + CROSSHAIR_CIRCLE_RADIUS, g_mouseY + CROSSHAIR_CIRCLE_RADIUS);

        SelectObject(hMemDC, hOldPen);
        DeleteObject(hLinePen);
    }

    // 플레이어 그리기
    HBRUSH hPlayerBrush = CreateSolidBrush(PLAYER_COLOR);
    RECT playerRect = { (int)g_playerX, (int)g_playerY, (int)g_playerX + PLAYER_SIZE, (int)g_playerY + PLAYER_SIZE };
    FillRect(hMemDC, &playerRect, hPlayerBrush);
    DeleteObject(hPlayerBrush);

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