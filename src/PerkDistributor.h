#pragma once

#include "pch.h"

namespace PerkDistributor
{
    bool IsNpcValid(RE::TESNPC* npc, bool excludePlayer);

    bool IsNpcValid(RE::TESNPC* npc);

    bool TryProcessNpc(RE::TESNPC* npc);

    bool TryRevertNpc(RE::TESNPC* npc);

    void ProcessMemoryActors();
}