#include "projectile.h"
#include "projectile_catalog.h"
#include "config.h"

// 오브젝트 풀
static Projectile s_projectiles[PROJECTILE_MAX_COUNT];

void Projectile_Init(void)
{
    for (int i = 0; i < PROJECTILE_MAX_COUNT; ++i)
    {
        s_projectiles[i].active = false;
    }
}

void Projectile_Update(float deltaTime, RECT clientRect)
{
    for (int i = 0; i < PROJECTILE_MAX_COUNT; ++i)
    {
        if (s_projectiles[i].active && s_projectiles[i].update)
        {
            s_projectiles[i].update(&s_projectiles[i], deltaTime, clientRect);
        }
    }
}

void Projectile_Render(HDC hdc)
{
    for (int i = 0; i < PROJECTILE_MAX_COUNT; ++i)
    {
        if (s_projectiles[i].active && s_projectiles[i].render)
        {
            s_projectiles[i].render(&s_projectiles[i], hdc);
        }
    }
}

void Projectile_Spawn(ProjectileType type, float startX, float startY, float dirX, float dirY)
{
    const ProjectileDesc* desc = ProjectileCatalog_GetDesc(type);
    if (!desc)
    {
        return;
    }

    for (int i = 0; i < PROJECTILE_MAX_COUNT; ++i)
    {
        // 오브젝트 풀링: !active인 배열에다가 새 객체 채워넣음
        if (!s_projectiles[i].active)
        {
            Projectile* p = &s_projectiles[i];
            p->active = true;
            p->x = startX;
            p->y = startY;

            // ProjectileDesc의 정적 속성을- 객체 내에 직접 복사.
            // ProjectileDesc* 포인터를 참조하는 간접참조 구조 대신,
            // 메모리를 조금 더 쓸지언정 배열에다 직접 속성을 집어넣어- 캐시 히트율 상승 (DOD)
            p->layer = desc->layer;
            p->mask = desc->mask;
            p->color = desc->color;
            p->size = desc->size;
            p->speed = desc->speed;
            p->update = desc->update;
            p->render = desc->render;

            // 투사체 종류별 고유 데이터 설정
            // 직선 투사체
            p->data.linear.dx = dirX;
            p->data.linear.dy = dirY;

            // 유도 투사체
            // ...

            return;
        }
    }
}

// --- 외부 모듈 상호작용 함수 ---

int Projectile_GetPoolSize(void)
{
    return PROJECTILE_MAX_COUNT;
}

// getter로 멤버변수 접근 대신, 구조체 포인터를 직접 넘김. const로 값 수정은 제한.
const Projectile* Projectile_GetFromPool(int index)
{
    return (index >= 0 && index < PROJECTILE_MAX_COUNT) ? &s_projectiles[index] : NULL;
}

void Projectile_Deactivate(int index)
{
    if (index >= 0 && index < PROJECTILE_MAX_COUNT)
    {
        s_projectiles[index].active = false;
    }
}