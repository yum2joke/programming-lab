#pragma once

#include "beam_types.h"

// type에 알맞은 빔 설계도를 리턴
const BeamDesc* BeamCatalog_GetDesc(AttackEntityType type);