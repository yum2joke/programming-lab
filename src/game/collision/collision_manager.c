#include "collision_manager.h"

#include "collision_types.h"
#include "config.h"
#include "game/entities/actors/boss/boss.h"
#include "game/entities/actors/player/player.h"
#include "game/entities/projectiles/projectile.h"
#include "game/entities/beams/beam.h"

#include <math.h>
#include <windows.h>

// OOP 대신 데이터 지향 설계(DOD) 기반 충돌 시스템.
// 배열 + struct 관리로 캐시 히트율 극대화
#define MAX_COLLIDABLES 600     // 투사체 + 레이저 + 아이템 + 파편 + 보스/플레이어/적 ...
static Collidable s_collidables[MAX_COLLIDABLES];
static int s_collidable_count = 0;

// 충돌 발생 시 결과 처리
static void HandleCollision(Collidable* a, Collidable* b)
{
    // 플레이어 총알 vs 보스
    if ((a->layer == LAYER_PLAYER_BULLET && b->layer == LAYER_BOSS) ||
        (a->layer == LAYER_BOSS && b->layer == LAYER_PLAYER_BULLET))
    {
        Collidable* bullet_collidable = (a->layer == LAYER_PLAYER_BULLET) ? a : b;
        
        Projectile_Deactivate(bullet_collidable->source_index);
        Boss_TakeDamage(BULLET_DAMAGE);
    }

    // 플레이어 vs 보스 또는 플레이어 vs 보스 총알
    if ((a->layer == LAYER_PLAYER && (b->layer == LAYER_BOSS || b->layer == LAYER_BOSS_BULLET)) ||
        (b->layer == LAYER_PLAYER && (a->layer == LAYER_BOSS || a->layer == LAYER_BOSS_BULLET)))
    {
        Player_TakeDamage(1);
    }
}

// 모든 활성 객체를 충돌체 목록으로 수집
static void GatherCollidables()
{
    s_collidable_count = 0;

    // 보스
    if (Boss_IsAlive() && s_collidable_count < MAX_COLLIDABLES)
    {
        s_collidables[s_collidable_count].layer = LAYER_BOSS;
        s_collidables[s_collidable_count].mask = LAYER_PLAYER_BULLET | LAYER_PLAYER;
        s_collidables[s_collidable_count].rect = Boss_GetRect();
        s_collidable_count++;
    }
    
    // 플레이어
    if (s_collidable_count < MAX_COLLIDABLES)
    {
        s_collidables[s_collidable_count].layer = LAYER_PLAYER;
        s_collidables[s_collidable_count].mask = LAYER_BOSS | LAYER_BOSS_BULLET;
        s_collidables[s_collidable_count].rect.left = (LONG)Player_GetX();
        s_collidables[s_collidable_count].rect.top = (LONG)Player_GetY();
        s_collidables[s_collidable_count].rect.right = (LONG)Player_GetX() + PLAYER_SIZE;
        s_collidables[s_collidable_count].rect.bottom = (LONG)Player_GetY() + PLAYER_SIZE;
        s_collidable_count++;
    }

    // 모든 투사체
    for (int i = 0; i < Projectile_GetPoolSize(); i++)
    {
        if (s_collidable_count >= MAX_COLLIDABLES) break;
        
        const Projectile* projectile = Projectile_GetFromPool(i);
        if (projectile && projectile->active)
        {
            s_collidables[s_collidable_count].layer = projectile->layer;
            s_collidables[s_collidable_count].mask = projectile->mask;
            
            // 충돌 영역 계산 (투사체의 x, y는 중심 좌표)
            s_collidables[s_collidable_count].rect.left = (LONG)projectile->x - projectile->size / 2;
            s_collidables[s_collidable_count].rect.top = (LONG)projectile->y - projectile->size / 2;
            s_collidables[s_collidable_count].rect.right = (LONG)projectile->x + projectile->size / 2;
            s_collidables[s_collidable_count].rect.bottom = (LONG)projectile->y + projectile->size / 2;
            
            s_collidables[s_collidable_count].source_index = i;
            s_collidable_count++;
        }
    }
}

// 빔(선분) - 플레이어(원) 최단 거리 기반 충돌 검사
// 충돌 알고리즘이 달라, 충돌체에 수집하지 않고 따로 처리
static void CheckBeamCollisions(void)
{
    int beamCount = Beam_GetPoolSize();
    for (int i = 0; i < beamCount; ++i)
    {
        const Beam* beam = Beam_GetFromPool(i);
        if (!beam || !beam->active) continue;

        // 발사 상태가 아니라면- 충돌 검사 X
        if (beam->state != BEAM_STATE_FIRING) continue;

        // 빔이 플레이어를 타격할 수 있는 마스크인지 확인
        if (!(beam->mask & LAYER_PLAYER)) continue;

        float px = Player_GetCenterX();
        float py = Player_GetCenterY();

        // 빔 시작점부터 플레이어 중심까지의 벡터
        float vx = px - beam->startX;
        float vy = py - beam->startY;

        // 빔 방향 벡터(dirX, dirY)에 내적(Dot Product)하여 빔 진행 방향으로의 투영 길이(t) 계산
        float t = (vx * beam->dirX) + (vy * beam->dirY);
        float closestX, closestY;

        if (t < 0.0f)
        {
            closestX = beam->startX;
            closestY = beam->startY;
        }
        else
        {
            closestX = beam->startX + (t * beam->dirX);
            closestY = beam->startY + (t * beam->dirY);
        }

        // 최단 거리 계산
        float dx = px - closestX;
        float dy = py - closestY;
        float distance = sqrtf(dx * dx + dy * dy);
        float collisionRadius = (beam->thickness / 2.0f) + (PLAYER_SIZE / 2.5f);

        // 충돌 판정 (거리가 반경보다 작으면 피격)
        if (distance < collisionRadius)
        {
            Player_TakeDamage(1);
        }
    }
}

void CollisionManager_CheckAll(void)
{
    // 매 프레임마다 충돌 검사할 충돌체 배열을 새로 구성
    GatherCollidables();

    // 빔 전용 수학적 충돌 검사
    CheckBeamCollisions();

    // 모든 충돌체 쌍을 검사 (Broad-phase)
    for (int i = 0; i < s_collidable_count; i++)
    {
        for (int j = i + 1; j < s_collidable_count; j++)
        {
            Collidable* obj_A = &s_collidables[i];
            Collidable* obj_B = &s_collidables[j];

            // LayerMask를 이용해 충돌 가능한지 확인
            if ((obj_A->mask & obj_B->layer) && (obj_B->mask & obj_A->layer))
            {
                // AABB 충돌 검사
                RECT intersection;
                if (IntersectRect(&intersection, &obj_A->rect, &obj_B->rect))   // Narrow-phase
                {
                    HandleCollision(obj_A, obj_B);
                }
            }
        }
    }
}