#include "player.h"
#include "config.h"

static float s_playerX, s_playerY;
static RECT s_clientRect;

void Player_Init(RECT clientRect)
{
    s_clientRect = clientRect;
    s_playerX = (float)(s_clientRect.right - PLAYER_SIZE) / 2.0f;
    s_playerY = (float)(s_clientRect.bottom - PLAYER_SIZE) / 2.0f;
}

void Player_Update(float deltaTime)
{
    float deltaX = 0.0f;
    float deltaY = 0.0f;

    deltaY -= PLAYER_SPEED * 0.1f;

    if (GetAsyncKeyState('A') & 0x8000) deltaX -= PLAYER_SPEED;
    if (GetAsyncKeyState('D') & 0x8000) deltaX += PLAYER_SPEED;
    if (GetAsyncKeyState('W') & 0x8000) deltaY -= PLAYER_SPEED;
    if (GetAsyncKeyState('S') & 0x8000)
    {
        float lowerLimitY = (float)s_clientRect.bottom - PLAYER_SIZE;
        if (s_playerY < lowerLimitY)
        {
            float ratio = (lowerLimitY - s_playerY) / lowerLimitY;
            deltaY += PLAYER_SPEED * ratio;
        }
    }

    s_playerX += deltaX * deltaTime;
    s_playerY += deltaY * deltaTime;

    if (s_playerX < s_clientRect.left) s_playerX = s_clientRect.left;
    if (s_playerX + PLAYER_SIZE > s_clientRect.right) s_playerX = (float)s_clientRect.right - PLAYER_SIZE;
    if (s_playerY < s_clientRect.top) s_playerY = s_clientRect.top;
    if (s_playerY + PLAYER_SIZE > s_clientRect.bottom) s_playerY = (float)s_clientRect.bottom - PLAYER_SIZE;
}

void Player_Render(HDC hdc)
{
    HBRUSH hPlayerBrush = CreateSolidBrush(PLAYER_COLOR);
    RECT playerRect = { (int)s_playerX, (int)s_playerY, (int)s_playerX + PLAYER_SIZE, (int)s_playerY + PLAYER_SIZE };
    FillRect(hdc, &playerRect, hPlayerBrush);
    DeleteObject(hPlayerBrush);
}

float Player_GetX(void) { return s_playerX; }
float Player_GetY(void) { return s_playerY; }
float Player_GetCenterX(void) { return s_playerX + PLAYER_SIZE / 2.0f; }
float Player_GetCenterY(void) { return s_playerY + PLAYER_SIZE / 2.0f; }