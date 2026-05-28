#include "player.h"
#include "config.h"

#include "game/attacks/attack.h"
#include "game/entities/actors/actor_manager.h"

#include <math.h>
#include <stdbool.h>

// 플레이어의 내부 상태 데이터
typedef struct {
    int actorId;
    float x;
    float y;
    int currentHP;
    int maxHP;
    RECT clientRect;
    
    float fireCooldown;
    bool isInvincible;
    float invincibleTimer;
    bool isAlive;
    int targetX;
    int targetY;
} Player;

static Player s_player;

void Player_Init(RECT clientRect)
{
    s_player.clientRect = clientRect;
    s_player.x = (float)(s_player.clientRect.right - PLAYER_SIZE) / 2.0f;
    s_player.y = (float)(s_player.clientRect.bottom - PLAYER_SIZE) / 2.0f;
    s_player.fireCooldown = 0.0f;
    s_player.actorId = ACTOR_ID_PLAYER;
    s_player.maxHP = PLAYER_MAX_HP;
    s_player.currentHP = s_player.maxHP;
    s_player.isInvincible = false;
    s_player.invincibleTimer = 0.0f;
    s_player.isAlive = true;
    s_player.targetX = 0;
    s_player.targetY = 0;

    ActorManager_RegisterPlayer();
}

// 플레이어 상태 무적으로
void Player_SetInvincible(float duration)
{
    s_player.isInvincible = true;
    s_player.invincibleTimer = duration;
}

static void Player_Death(void)
{
    s_player.isAlive = false;
    ActorManager_Remove(s_player.actorId);

    // TODO: 파괴되는 모션 추가
}

void Player_TakeDamage(int damage)
{
    if (!s_player.isAlive || s_player.isInvincible) return;

    s_player.currentHP -= damage;
    if (s_player.currentHP <= 0)
    {
        s_player.currentHP = 0;
        Player_Death();
    }
    else
    {
        Player_SetInvincible(1.0f); // 피격 시 1초간 무적
    }
}

void Player_Update(float deltaTime, int mouseX, int mouseY)
{
    if (!s_player.isAlive) return;

    // 마우스 좌표 기억
    s_player.targetX = mouseX;
    s_player.targetY = mouseY;

    if (s_player.isInvincible)
    {
        s_player.invincibleTimer -= deltaTime;
        if (s_player.invincibleTimer <= 0.0f)
        {
            s_player.isInvincible = false;
            s_player.invincibleTimer = 0.0f;
        }
    }

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

bool Player_IsAlive(void)
{
    return s_player.isAlive;
}

void Player_Render(HDC hdc)
{
    if (!s_player.isAlive) return;

    // 무적 상태일 경우 회색으로, 아닐 경우 원래 색상으로 렌더링
    COLORREF color = s_player.isInvincible ? RGB(128, 128, 128) : PLAYER_COLOR;
    HBRUSH hPlayerBrush = CreateSolidBrush(color);
    RECT playerRect = { (int)s_player.x, (int)s_player.y, (int)s_player.x + PLAYER_SIZE, (int)s_player.y + PLAYER_SIZE };
    FillRect(hdc, &playerRect, hPlayerBrush);
    DeleteObject(hPlayerBrush);

    // 조준선 그리기
    HPEN hLinePen = CreatePen(PS_SOLID, CROSSHAIR_LINE_THICKNESS, CROSSHAIR_LINE_COLOR);
    HPEN hOldPen = (HPEN)SelectObject(hdc, hLinePen);

    float playerCenterX = Player_GetCenterX();
    float playerCenterY = Player_GetCenterY();
    MoveToEx(hdc, (int)playerCenterX, (int)playerCenterY, NULL);

    float dirX = (float)s_player.targetX - playerCenterX;
    float dirY = (float)s_player.targetY - playerCenterY;
    float length = sqrtf(dirX * dirX + dirY * dirY);

    if (length > 0.001f)
    {
        float endX = playerCenterX + (dirX / length) * 2000.0f;
        float endY = playerCenterY + (dirY / length) * 2000.0f;
        LineTo(hdc, (int)endX, (int)endY);
    }

    Ellipse(hdc, s_player.targetX - CROSSHAIR_CIRCLE_RADIUS, s_player.targetY - CROSSHAIR_CIRCLE_RADIUS, s_player.targetX + CROSSHAIR_CIRCLE_RADIUS, s_player.targetY + CROSSHAIR_CIRCLE_RADIUS);

    SelectObject(hdc, hOldPen);
    DeleteObject(hLinePen);
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