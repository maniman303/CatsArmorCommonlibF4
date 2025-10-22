#include "PerkDistributor.h"
#include "FormUtil.h"
#include "Setup.h"
#include "Files.h"

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

    bool IsNpcValid(RE::TESNPC* npc, bool excludePlayer)
    {
        if (!Files::IsFilePresent() || !Setup::IsInitialized())
        {
            return false;
        }

        if (npc == NULL)
        {
            // REX::INFO("Npc is null");
            return false;
        }

        if ((npc->IsPlayer() && excludePlayer) || npc->IsDeleted() || npc->IsDisabled())
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

        return true;
    }

    bool IsNpcValid(RE::TESNPC* npc)
    {
        return IsNpcValid(npc, true);
    }

    bool TryProcessNpc(RE::TESNPC* npc)
    {
        if (!IsNpcValid(npc))
        {
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

        bool res = true;

        if (!npc->GetSpellList()->AddSpells(std::vector<RE::SpellItem*>{spell}))
        {
            REX::WARN("Failed to add spell.");
            res = false;
        }

        if (!npc->AddPerks(std::vector<RE::BGSPerk*>{perk}, 1))
        {
            REX::WARN("Failed to add perk.");
            res = false;
        }

        if (res)
        {
            // REX::INFO("Added spell and perk to [{}]", npc->GetFullName());
        }

        return res;
    }

    bool TryRevertNpc(RE::TESNPC* npc)
    {
        if (!IsNpcValid(npc))
        {
            return false;
        }

        auto spell = Setup::GetSpell();
        auto perk = Setup::GetPerk();

        if (spell == NULL || perk == NULL)
        {
            return false;
        }

        auto perkIndex = npc->GetPerkIndex(perk);
        if (perkIndex.has_value() && perkIndex.value() > 0)
        {
            npc->RemovePerks(std::vector<RE::BGSPerk*>{perk});
        }

        npc->GetSpellList()->RemoveSpells(std::vector<RE::SpellItem*>{spell});

        // REX::INFO("Removed spell and perk from [{}]", npc->GetFullName());

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