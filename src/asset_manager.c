#include "asset_manager.h"

#include <stddef.h>

static GpImage* s_images[ASSET_COUNT] = { NULL };

void AssetManager_Init(void)
{
    // 실행 파일 기준으로 ../assets 경로에서 이미지 로드
    GdipLoadImageFromFile(L"../assets/player.png", &s_images[ASSET_PLAYER]);
    GdipLoadImageFromFile(L"../assets/bossA.png", &s_images[ASSET_BOSS_A]);
}

void AssetManager_Cleanup(void)
{
    for (int i = 0; i < ASSET_COUNT; ++i)
    {
        if (s_images[i])
        {
            GdipDisposeImage(s_images[i]);
            s_images[i] = NULL;
        }
    }
}

GpImage* AssetManager_GetImage(AssetType type)
{
    if (type >= 0 && type < ASSET_COUNT)
    {
        return s_images[type];
    }
    return NULL;
}