#pragma once

#include "pch.h"

namespace PerkDistributor
{
    bool TryProcessNpc(RE::TESNPC* npc);

    bool TryRevertNpc(RE::TESNPC* npc);

    void ProcessMemoryActors();
}