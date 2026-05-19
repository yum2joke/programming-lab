#include <stdbool.h>

#include "collision_manager.h"
#include "config.h"

#include "game/entities/bullet/bullet.h"
#include "game/entities/boss/boss.h"

#define MAX_COLLIDABLES 200     // 게임 내 최대 충돌체 수
static Collidable s_collidables[MAX_COLLIDABLES];   // 충돌체 담는 배열
static int s_collidable_count = 0;

// 충돌 발생 시 결과 처리
static void HandleCollision(Collidable* a, Collidable* b)
{
    // 플레이어 총알 vs 보스
    if ((a->layer == LAYER_PLAYER_BULLET && b->layer == LAYER_BOSS) ||
        (a->layer == LAYER_BOSS && b->layer == LAYER_PLAYER_BULLET))
    {
        // 충돌한 객체 중 총알 찾기
        Collidable* bullet_collidable = (a->layer == LAYER_PLAYER_BULLET) ? a : b;
        
        Bullet_Deactivate(bullet_collidable->source_index); // 총알 비활성화
        Boss_TakeDamage(BULLET_DAMAGE);
    }
    // TODO: 다른 충돌 결과 처리 로직 추가
}

// 모든 활성 객체를 충돌체 목록으로 수집
static void GatherCollidables()
{
    s_collidable_count = 0;

    // 보스
    if (Boss_IsAlive() && s_collidable_count < MAX_COLLIDABLES)
    {
        s_collidables[s_collidable_count].layer = LAYER_BOSS;
        s_collidables[s_collidable_count].mask = LAYER_PLAYER_BULLET;   // 보스 - 플레이어 총알
        s_collidables[s_collidable_count].rect = Boss_GetRect();
        s_collidable_count++;
    }

    // 플레이어 총알 (다른 총알을 어찌할지가 고민이 되네)
    for (int i = 0; i < BULLET_MAX_COUNT; i++)
    {
        if (s_collidable_count >= MAX_COLLIDABLES) break;
        if (Bullet_IsActive(i))
        {
            s_collidables[s_collidable_count].layer = LAYER_PLAYER_BULLET;
            s_collidables[s_collidable_count].mask = LAYER_BOSS;        // 플레이어 총알 - 보스
            
            LONG x, y;
            Bullet_GetPosition(i, &x, &y);
            s_collidables[s_collidable_count].rect.left = x - BULLET_SIZE / 2;
            s_collidables[s_collidable_count].rect.top = y - BULLET_SIZE / 2;
            s_collidables[s_collidable_count].rect.right = x + BULLET_SIZE / 2;
            s_collidables[s_collidable_count].rect.bottom = y + BULLET_SIZE / 2;
            
            s_collidables[s_collidable_count].source_index = i;
            s_collidable_count++;
        }
    }

    // TODO: 다른 객체들 추가
}

void CollisionManager_CheckAll(void)
{
    // 매 프레임마다 충돌 검사할 객체(=충돌체) 목록을 새로 구성
    GatherCollidables();

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