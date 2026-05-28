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

// --- Narrow-phase 충돌 알고리즘 ---

// 현재 필요X
static bool CheckCollision_AABB_AABB(const RECT* a, const RECT* b)
{
    RECT intersection;
    return IntersectRect(&intersection, a, b) != 0;
}

static bool CheckCollision_Sphere_Sphere(const Sphere* a, const Sphere* b)
{
    float dx = a->x - b->x;
    float dy = a->y - b->y;
    float distanceSq = dx * dx + dy * dy;
    float radiusSum = a->radius + b->radius;
    
    // sqrt(제곱근) 연산을 피하기 위해 반지름의 합을 제곱하여 비교합니다.
    return distanceSq <= (radiusSum * radiusSum);
}

static bool CheckCollision_Capsule_Sphere(float capX, float capY, float capDirX, float capDirY, float capLength, float capRadius, const Sphere* sphere)
{
    float vx = sphere->x - capX;
    float vy = sphere->y - capY;

    float t = (vx * capDirX) + (vy * capDirY);
    
    // 선분 길이 내로 제한 (Clamping)
    if (t < 0.0f) t = 0.0f;
    else if (t > capLength) t = capLength;

    float closestX = capX + (t * capDirX);
    float closestY = capY + (t * capDirY);

    float dx = sphere->x - closestX;
    float dy = sphere->y - closestY;
    
    // sqrtf 대신 제곱 비교로 성능 최적화
    float distanceSq = dx * dx + dy * dy;
    float radiusSum = capRadius + sphere->radius;

    return distanceSq <= (radiusSum * radiusSum);
}

// --- 전체 충돌체 관리 함수 ---

// 모든 활성 객체를 충돌체 목록으로 수집
static void GatherCollidables()
{
    s_collidable_count = 0;

    // 보스
    if (Boss_IsAlive() && s_collidable_count < MAX_COLLIDABLES)
    {
        s_collidables[s_collidable_count].layer = LAYER_BOSS;
        s_collidables[s_collidable_count].mask = LAYER_PLAYER_BULLET | LAYER_PLAYER;
        s_collidables[s_collidable_count].shape = SHAPE_SPHERE;
        s_collidables[s_collidable_count].data.sphere.x = Boss_GetCenterX();
        s_collidables[s_collidable_count].data.sphere.y = Boss_GetCenterY();
        s_collidables[s_collidable_count].data.sphere.radius = BOSS_WIDTH / 2.0f;
        s_collidable_count++;
    }
    
    // 플레이어
    if (Player_IsAlive() && !Player_IsInvincible() && s_collidable_count < MAX_COLLIDABLES)
    {
        s_collidables[s_collidable_count].layer = LAYER_PLAYER;
        s_collidables[s_collidable_count].mask = LAYER_BOSS | LAYER_ENEMY_ATTACK;
        s_collidables[s_collidable_count].shape = SHAPE_SPHERE;
        s_collidables[s_collidable_count].data.sphere.x = Player_GetCenterX();
        s_collidables[s_collidable_count].data.sphere.y = Player_GetCenterY();
        s_collidables[s_collidable_count].data.sphere.radius = PLAYER_SIZE / 2.0f;
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
            s_collidables[s_collidable_count].shape = projectile->shape;
            
            if (projectile->shape == SHAPE_SPHERE)
            {
                s_collidables[s_collidable_count].data.sphere.x = projectile->x;
                s_collidables[s_collidable_count].data.sphere.y = projectile->y;
                s_collidables[s_collidable_count].data.sphere.radius = projectile->collisionRadius;
            }
            else if (projectile->shape == SHAPE_AABB)
            {
                s_collidables[s_collidable_count].data.aabb.left = (LONG)projectile->x - projectile->size / 2;
                s_collidables[s_collidable_count].data.aabb.top = (LONG)projectile->y - projectile->size / 2;
                s_collidables[s_collidable_count].data.aabb.right = (LONG)projectile->x + projectile->size / 2;
                s_collidables[s_collidable_count].data.aabb.bottom = (LONG)projectile->y + projectile->size / 2;
            }
            
            s_collidables[s_collidable_count].source_index = i;
            s_collidable_count++;
        }
    }

    // 모든 빔
    for (int i = 0; i < Beam_GetPoolSize(); i++)
    {
        if (s_collidable_count >= MAX_COLLIDABLES) break;

        const Beam* beam = Beam_GetFromPool(i);
        if (beam && beam->active && beam->state == BEAM_STATE_FIRING)
        {
            s_collidables[s_collidable_count].layer = beam->layer;
            s_collidables[s_collidable_count].mask = beam->mask;
            s_collidables[s_collidable_count].shape = beam->shape;
            
            if (beam->shape == SHAPE_CAPSULE)
            {
                s_collidables[s_collidable_count].data.capsule.x = beam->startX;
                s_collidables[s_collidable_count].data.capsule.y = beam->startY;
                s_collidables[s_collidable_count].data.capsule.dirX = beam->dirX;
                s_collidables[s_collidable_count].data.capsule.dirY = beam->dirY;
                s_collidables[s_collidable_count].data.capsule.length = 2500.0f;
                s_collidables[s_collidable_count].data.capsule.radius = beam->thickness / 2.0f;
            }
            s_collidables[s_collidable_count].source_index = i; // 빔은 독립된 생명주기가 있어, source_index는 현재 미사용
            s_collidable_count++;
        }
    }
}

// 충돌체의 shape 따져, 알맞은 충돌 알고리즘 호출
static bool CheckCollision(const Collidable* a, const Collidable* b)
{
    // 항상 a->shape의 CollisionShape enum이 작도록 swap
    if (a->shape > b->shape)
    {
        const Collidable* temp = a;
        a = b;
        b = temp;
    }

    if (a->shape == SHAPE_AABB && b->shape == SHAPE_AABB)
    {
        return CheckCollision_AABB_AABB(&a->data.aabb, &b->data.aabb);
    }
    else if (a->shape == SHAPE_SPHERE && b->shape == SHAPE_SPHERE)
    {
        return CheckCollision_Sphere_Sphere(&a->data.sphere, &b->data.sphere);
    }
    else if (a->shape == SHAPE_SPHERE && b->shape == SHAPE_CAPSULE)
    {
        const Sphere* sph = &a->data.sphere;
        const Capsule* cap = &b->data.capsule;
        return CheckCollision_Capsule_Sphere(cap->x, cap->y, cap->dirX, cap->dirY, cap->length, cap->radius, sph);
    }
    // TODO: AABB vs Sphere 충돌 구현-?

    return false; 
}

// 충돌 발생 시 결과 처리
static void HandleCollision(Collidable* a, Collidable* b)
{
    // 항상 a->shape의 CollisionShape enum이 작도록 swap
    if (a->layer > b->layer)
    {
        Collidable* temp = a;
        a = b;
        b = temp;
    }

    // 플레이어 총알 vs 보스
    if (a->layer == LAYER_PLAYER_BULLET && b->layer == LAYER_BOSS)
    {
        Projectile_Deactivate(a->source_index);
        Boss_TakeDamage(BULLET_DAMAGE);
    }

    // 플레이어 vs 보스 또는 플레이어 vs 적 공격entity
    if (a->layer == LAYER_PLAYER && (b->layer == LAYER_BOSS || b->layer == LAYER_ENEMY_ATTACK))
    {
        Player_TakeDamage(1);

        // 적 공격entity가 !passThrough인 투사체라면 소멸
        if (b->layer == LAYER_ENEMY_ATTACK && b->shape == SHAPE_SPHERE)
        {
            const Projectile* proj = Projectile_GetFromPool(b->source_index);
            if (proj && !proj->passThrough)
            {
                Projectile_Deactivate(b->source_index);
            }
        }
    }
}

void CollisionManager_CheckAll(void)
{
    // 매 프레임마다 충돌 검사할 충돌체 배열을 새로 구성
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
                // 어떤 충돌체들인지, 정말로 충돌한 것인지 판단 (Narrow-phase)
                if (CheckCollision(obj_A, obj_B))
                {
                    HandleCollision(obj_A, obj_B);
                }
            }
        }
    }
}