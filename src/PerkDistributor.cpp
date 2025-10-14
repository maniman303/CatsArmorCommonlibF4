#include "PerkDistributor.h"
#include "FormUtil.h"
#include "Setup.h"

namespace PerkDistributor
{
    bool IsRaceOfHumanOrigin(RE::TESRace* race)
    {
        if (race == NULL)
        {
            return false;
        }

        auto humanRace = FormUtil::GetFormFromMod("Fallout4.esm", 0x00013746);
        if (race == humanRace)
        {
            return true;
        }

        auto armorRace = race->armorParentRace;

        return IsRaceOfHumanOrigin(armorRace);
    }

    bool TryProcessNpc(RE::TESNPC* npc)
    {
        if (npc == NULL)
        {
            // REX::INFO("Npc is null");
            return false;
        }

        if (npc->IsPlayer() || npc->IsDeleted() || npc->IsSimpleActor())
        {
            // REX::INFO("Npc doesn't match criteria... [{}]", npc->GetFullName());
            return false;
        }

        auto race = npc->GetFormRace();
        if (!IsRaceOfHumanOrigin(race))
        {
            // REX::INFO("Npc is not human... [{}]", npc->GetFullName());
            return false;
        }

        // REX::INFO("Found human! [{}]", npc->GetFullName());

        auto spell = Setup::GetSpell();
        auto perk = Setup::GetPerk();

        if (spell == NULL || perk == NULL)
        {
            return false;
        }

        auto perkIndex = npc->GetPerkIndex(perk);
        if (perkIndex.has_value() && perkIndex.value() > 0)
        {
            return false;
        }

        npc->GetSpellList()->AddSpells({spell});
        npc->AddPerks({perk}, 1);

        return true;
    }

    void ProcessMemoryActors()
    {
        auto processLists = RE::ProcessLists::GetSingleton();

        int processedNpcs = 0;

        for (const auto& actorHandle : processLists->lowActorHandles)
        {
            const auto& actor = actorHandle.get();
            if (actor == NULL)
            {
                continue;
            }

            auto npc = actor->GetNPC();
            if (npc == NULL)
            {
                continue;
            }

            if (TryProcessNpc(npc))
            {
                processedNpcs++;
            }
        }

        REX::INFO("Processed {} npcs.", processedNpcs);
    }
}