#include "player.h"
#include "config.h"

#include "game/attacks/attack.h"
#include "game/entities/actors/actor_manager.h"

#include <math.h>

// 플레이어의 내부 상태 데이터
typedef struct {
    int actorId;
    float x;
    float y;
    float fireCooldown;
    RECT clientRect;
} Player;

static Player s_player;

void Player_Init(RECT clientRect)
{
    s_player.clientRect = clientRect;
    s_player.x = (float)(s_player.clientRect.right - PLAYER_SIZE) / 2.0f;
    s_player.y = (float)(s_player.clientRect.bottom - PLAYER_SIZE) / 2.0f;
    s_player.fireCooldown = 0.0f;
    s_player.actorId = ACTOR_ID_PLAYER;

    ActorManager_RegisterPlayer();
}

// TODO: 플레이어 죽음 제대로 구현하기

void Player_Update(float deltaTime, int mouseX, int mouseY)
{
    // --- 발사 로직 ---
    s_player.fireCooldown -= deltaTime;
    if (s_player.fireCooldown <= 0.0f)
    {
        s_player.fireCooldown = FIRE_RATE; // 쿨다운 초기화

        float playerCenterX = Player_GetCenterX();
        float playerCenterY = Player_GetCenterY();
        float dirX = mouseX - playerCenterX;
        float dirY = mouseY - playerCenterY;
        float length = sqrtf(dirX * dirX + dirY * dirY);

        if (length > 0.001f)
        {
            Attack_SingleShot(s_player.actorId, playerCenterX, playerCenterY, dirX / length, dirY / length, PROJECTILE_PLAYER_BULLET);
        }
        else    // 마우스가 플레이어 위에 있을 경우, 위로 발사
        {
            Attack_SingleShot(s_player.actorId, playerCenterX, playerCenterY, 0.0f, -1.0f, PROJECTILE_PLAYER_BULLET);
        }
    }

    // --- 이동 로직 ---
    float deltaX = 0.0f;
    float deltaY = 0.0f;

    deltaY -= PLAYER_SPEED * 0.1f;

    if (GetAsyncKeyState('A') & 0x8000) deltaX -= PLAYER_SPEED;
    if (GetAsyncKeyState('D') & 0x8000) deltaX += PLAYER_SPEED;
    if (GetAsyncKeyState('W') & 0x8000) deltaY -= PLAYER_SPEED;
    if (GetAsyncKeyState('S') & 0x8000)
    {
        float lowerLimitY = (float)s_player.clientRect.bottom - PLAYER_SIZE;
        if (s_player.y < lowerLimitY)
        {
            float ratio = (lowerLimitY - s_player.y) / lowerLimitY;
            deltaY += PLAYER_SPEED * ratio;
        }
    }

    s_player.x += deltaX * deltaTime;
    s_player.y += deltaY * deltaTime;

    if (s_player.x < s_player.clientRect.left) s_player.x = (float)s_player.clientRect.left;
    if (s_player.x + PLAYER_SIZE > s_player.clientRect.right) s_player.x = (float)s_player.clientRect.right - PLAYER_SIZE;
    if (s_player.y < s_player.clientRect.top) s_player.y = (float)s_player.clientRect.top;
    if (s_player.y + PLAYER_SIZE > s_player.clientRect.bottom) s_player.y = (float)s_player.clientRect.bottom - PLAYER_SIZE;

    ActorManager_UpdatePosition(s_player.actorId, Player_GetCenterX(), Player_GetCenterY());
}

void Player_Render(HDC hdc)
{
    HBRUSH hPlayerBrush = CreateSolidBrush(PLAYER_COLOR);
    RECT playerRect = { (int)s_player.x, (int)s_player.y, (int)s_player.x + PLAYER_SIZE, (int)s_player.y + PLAYER_SIZE };
    FillRect(hdc, &playerRect, hPlayerBrush);
    DeleteObject(hPlayerBrush);
}

float Player_GetX(void)
{
    return s_player.x;
}
float Player_GetY(void)
{
    return s_player.y;
}
float Player_GetCenterX(void)
{
    return s_player.x + PLAYER_SIZE / 2.0f;
}
float Player_GetCenterY(void)
{
    return s_player.y + PLAYER_SIZE / 2.0f;
}