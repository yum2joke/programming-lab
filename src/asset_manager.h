#pragma once

#include "gdiplus_c.h"

typedef enum {
    ASSET_PLAYER,
    ASSET_BOSS_A,
    ASSET_COUNT
} AssetType;

// 에셋 매니저 초기화 및 해제
void AssetManager_Init(void);
void AssetManager_Cleanup(void);

// 이미지 반환
GpImage* AssetManager_GetImage(AssetType type);