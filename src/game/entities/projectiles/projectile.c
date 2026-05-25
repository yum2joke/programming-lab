#include "projectile.h"
#include "config.h"

// 오브젝트 풀
static Projectile s_projectiles[PROJECTILE_MAX_COUNT];

// --- 업데이트 함수 ---

// 직선 투사체 업데이트
static void UpdateProjectile_Linear(Projectile* self, float deltaTime, RECT clientRect)
{
    self->x += self->data.linear.dx * self->speed * deltaTime;
    self->y += self->data.linear.dy * self->speed * deltaTime;

    // 화면 밖으로 나가면 비활성화
    if (self->x < -self->size || self->x > clientRect.right + self->size ||
        self->y < -self->size || self->y > clientRect.bottom + self->size)
    {
        self->active = false;
    }
}

// --- 렌더링 함수 ---

// 사각형 렌더링
static void RenderProjectile_Default(const Projectile* self, HDC hdc)
{
    HBRUSH hBrush = CreateSolidBrush(self->color);
    RECT rect = {
        (int)self->x - self->size / 2,
        (int)self->y - self->size / 2,
        (int)self->x + self->size / 2,
        (int)self->y + self->size / 2
    };
    FillRect(hdc, &rect, hBrush);
    DeleteObject(hBrush);
}

// --- 정적 속성 설계도 ---

// 플레이어 총알
static const ProjectileDesc PLAYER_BULLET_DESC = {
    // 지정 초기자 문법
    .layer = LAYER_PLAYER_BULLET,
    .mask = LAYER_BOSS,
    .speed = BULLET_SPEED,
    .color = BULLET_COLOR,
    .size = BULLET_SIZE,
    .update = UpdateProjectile_Linear,
    .render = RenderProjectile_Default
};

// 보스 총알
static const ProjectileDesc BOSS_BULLET_DESC = {
    .layer = LAYER_BOSS_BULLET,
    .mask = LAYER_PLAYER,
    .speed = BOSS_BULLET_SPEED,
    .color = BOSS_BULLET_COLOR,
    .size = BULLET_SIZE,
    .update = UpdateProjectile_Linear,
    .render = RenderProjectile_Default
};

// --- 공개 API ---

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

// --- 생성 팩토리 함수 ---

// 범용 생성 함수
static void CreateProjectile(const ProjectileDesc* desc, float startX, float startY, float dirX, float dirY)
{
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

// 외부에서 호출하는 투사체별 생성 함수
// 플레이어 총알
void Projectile_CreatePlayerBullet(float startX, float startY, float dirX, float dirY)
{
    CreateProjectile(&PLAYER_BULLET_DESC, startX, startY, dirX, dirY);
}

// 보스 총알
void Projectile_CreateBossBullet(float startX, float startY, float dirX, float dirY)
{
    CreateProjectile(&BOSS_BULLET_DESC, startX, startY, dirX, dirY);
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
    if (index >= 0 && index < PROJECTILE_MAX_COUNT) s_projectiles[index].active = false;
}