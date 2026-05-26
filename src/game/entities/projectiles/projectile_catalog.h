#pragma once

#include "projectile_types.h"

// type에 알맞은 투사체 설계도 리턴
const ProjectileDesc* ProjectileCatalog_GetDesc(ProjectileType type);