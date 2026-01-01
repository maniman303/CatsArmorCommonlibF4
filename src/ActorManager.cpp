#include "ActorManager.h"
#include "Setup.h"

uint32_t CountStacks(const RE::BGSInventoryItem& itemData)
{
    uint32_t res = 0;

    auto stack = itemData.stackData.get();
    while (stack != NULL)
    {
        res++;
        stack = stack->nextStack.get();
    }

    return res;
}

bool ActorManager::WornHasKeyword(RE::Actor* actor, RE::BGSKeyword* keyword)
{
    if (actor == NULL || keyword == NULL)
    {
        return false;
    }

    auto inventoryList = actor->inventoryList;
    if (inventoryList == NULL)
    {
        REX::WARN(std::format("Inventory for actor [{0}] is NULL.", actor->GetDisplayFullName()));
        return false;
    }

    for (const auto& itemData : inventoryList->data)
    {
        auto object = itemData.object;
        if (object == NULL)
        {
            continue;
        }

        if (!object->Is<RE::TESObjectARMO>())
        {
            continue;
        }

        auto armor = object->As<RE::TESObjectARMO>();

        for (uint32_t i = 0; i < CountStacks(itemData); i++)
        {
            auto stack = itemData.GetStackByID(i);
            if (stack == NULL)
            {
                // REX::INFO("Stack is null, continued.");
                continue;
            }

            // REX::INFO("Has stack.");

            if (!stack->IsEquipped())
            {
                // REX::INFO("Continued.");
                continue;
            }

            // REX::INFO("Is equipped.");

            if (armor->HasKeyword(keyword))
            {
                return true;
            }

            auto instance = itemData.GetInstanceData(i);
            if (instance == NULL)
            {
                // REX::INFO("Continued.");
                continue;
            }

            // REX::INFO("Has instance data.");

            auto keywordData = instance->GetKeywordData();
            if (keywordData == NULL)
            {
                // REX::INFO("Continued.");
                continue;
            }

            // REX::INFO("Has keyword data.");

            auto keywordIndex = keywordData->GetKeywordIndex(keyword);
            if (keywordIndex.has_value())
            {
                return true;
            }

            // REX::INFO("Didn't have keyword index.");
        }

        // REX::INFO("Next item.");
    }

    return false;
}

bool ActorManager::IsItemEquipped(RE::Actor* actor, RE::BGSObjectInstance instance)
{
    if (actor == NULL || instance.object == NULL)
    {
        return false;
    }

    for (auto itemData : actor->inventoryList->data)
    {
        auto object = itemData.object;
        if (object == NULL || object != instance.object)
        {
            continue;
        }

        for (uint32_t i = 0; i < CountStacks(itemData); i++)
        {
            auto stack = itemData.GetStackByID(i);
            if (stack == NULL)
            {
                continue;
            }

            if (!stack->IsEquipped())
            {
                continue;
            }

            if (!instance.instanceData)
            {
                return true;
            }

            auto expectedInstanceData = instance.instanceData.get();
            if (itemData.GetInstanceData(i) == expectedInstanceData)
            {
                return true;
            }
        }
    }

    return false;
}

bool ActorManager::ProcessHairStubs(RE::Actor* actor, RE::BGSObjectInstance armor, bool isUnequipEvent)
{
    auto setup = Setup::GetForms("headgear");
    if (setup.isEmpty)
    {
        return true;
    }

    // REX::INFO("Process hair stubs, setup is valid.");

    bool isVisibleHelmetWorn = ActorManager::WornHasKeyword(actor, setup.keyword) &&
        !ActorManager::WornHasKeyword(actor, setup.keywordHidden);

    // REX::INFO("Worn keyboards checked.");

    bool isEquipped = ActorManager::IsItemEquipped(actor, armor);

    // REX::INFO("Is equipped checked.");

    if (!isUnequipEvent && !isEquipped)
    {
        // REX::INFO("Process hair stubs quick return.");

        // Skip broken events
        return false;
    }

    // REX::INFO(std::format("Analyze is visible: {0}, is unequip: {1}, is equipped: {2}, form id: {3}", isVisibleHelmetWorn, isUnequipEvent, isEquipped, armor.object->GetFormID()));

    auto instanceHairTop = RE::BGSObjectInstance(setup.armorHairTop, NULL);
    auto instanceHairLong = RE::BGSObjectInstance(setup.armorHairLong, NULL);
    auto instanceHairBeard = RE::BGSObjectInstance(setup.armorHairBeard, NULL);

    auto equipManager = RE::ActorEquipManager::GetSingleton();

    // REX::INFO("Equip manager set up.");

    if (!isVisibleHelmetWorn || !isEquipped)
    {
        // REX::INFO("Process unequip.");

        equipManager->UnequipObject(actor, &instanceHairTop, 1, NULL, 0, true, true, false, true, NULL);
        equipManager->UnequipObject(actor, &instanceHairLong, 1, NULL, 0, true, true, false, true, NULL);
        equipManager->UnequipObject(actor, &instanceHairBeard, 1, NULL, 0, true, true, false, true, NULL);

        // REX::INFO("Process unequip completed.");

        actor->Reset3D(true, 0, true, 0xC);

        // REX::INFO("3D reset completed.");

        return isUnequipEvent != isEquipped;
    }

    // REX::INFO("Process equip.");

    bool res = true;

    if (ActorManager::WornHasKeyword(actor, setup.keywordHairTop))
    {
        res = res && equipManager->EquipObject(actor, instanceHairTop, 0, 1, NULL, true, true, false, true, true);
    }

    if (ActorManager::WornHasKeyword(actor, setup.keywordHairLong))
    {
        res = res && equipManager->EquipObject(actor, instanceHairLong, 0, 1, NULL, true, true, false, true, true);
    }

    if (ActorManager::WornHasKeyword(actor, setup.keywordHairBeard))
    {
        res = res && equipManager->EquipObject(actor, instanceHairBeard, 0, 1, NULL, true, true, false, true, true);
    }

    // REX::INFO("Process equip completed.");

    actor->Reset3D(true, 0, true, 0xC);

    // REX::INFO("3D reset completed.");

    return res && (isUnequipEvent != isEquipped);
}