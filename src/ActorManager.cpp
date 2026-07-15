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

    for (auto& itemData : inventoryList->data)
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

bool ActorManager::IsItemEquipped(RE::Actor* actor, const RE::BGSObjectInstance* instance)
{
    if (actor == NULL || instance == NULL || instance->object == NULL)
    {
        return false;
    }

    if (!instance->instanceData)
    {
        return true;
    }

    for (auto itemData : actor->inventoryList->data)
    {
        auto object = itemData.object;
        if (object == NULL || object != instance->object)
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

            auto expectedInstanceData = instance->instanceData.get();
            if (itemData.GetInstanceData(i) == expectedInstanceData)
            {
                return true;
            }
        }
    }

    return false;
}

bool ActorManager::UnequipItem(RE::Actor* actor, RE::TESObjectARMO* armor)
{
    if (actor == NULL || armor == NULL)
    {
        return false;
    }

    auto equipManager = RE::ActorEquipManager::GetSingleton();
    if (actor->GetInventoryObjectCount(armor) <= 0)
    {
        return true;
    }

    for (auto itemData : actor->inventoryList->data)
    {
        auto object = itemData.object;
        if (object == NULL)
        {
            continue;
        }

        if (itemData.GetCount() <= 0)
        {
            continue;
        }

        if (object != armor)
        {
            continue;
        }

        for (uint32_t i = 0; i < CountStacks(itemData); i++)
        {
            auto stack = itemData.GetStackByID(i);
            if (!stack->IsEquipped())
            {
                continue;
            }

            auto instanceData = itemData.GetInstanceData(i);
            if (instanceData == NULL)
            {
                instanceData = &armor->armorData;
            }

            auto instance = new RE::BGSObjectInstance(armor, instanceData);
            return equipManager->UnequipObject(actor, instance, 1, armor->equipSlot, i, false, true, false, true, NULL);
        }
    }

    return true;
}

bool ActorManager::EquipItem(RE::Actor* actor, RE::TESObjectARMO* armor)
{
    if (actor == NULL || armor == NULL || actor->inventoryList == NULL)
    {
        return false;
    }

    auto itemCount = actor->GetInventoryObjectCount(armor);
    // if (itemCount > 0)
    // {
    //     RE::TESObjectREFR::RemoveItemData removeItemData(armor, static_cast<int32_t>(itemCount));
    //     actor->RemoveItem(removeItemData);
    //     itemCount = 0;
    // }

    if (itemCount == 0)
    {
        auto equipIndex = RE::BGSEquipIndex();
        equipIndex.index = 0;

        actor->inventoryList->rwLock.lock_write();

        actor->inventoryList->AddItem2(armor->As<RE::TESBoundObject>(), 1, new RE::ExtraDataList(), 0);

        actor->inventoryList->rwLock.unlock_write();
    }

    auto equipManager = RE::ActorEquipManager::GetSingleton();
    for (auto itemData : actor->inventoryList->data)
    {
        auto object = itemData.object;
        if (object == NULL)
        {
            continue;
        }

        if (itemData.GetCount() <= 0)
        {
            continue;
        }

        if (object != armor)
        {
            continue;
        }

        if (CountStacks(itemData) > 0)
        {
            auto stack = itemData.GetStackByID(0);
            if (stack->IsEquipped())
            {
                REX::INFO("Item already equipped.");
                return true;
            }

            auto instanceData = itemData.GetInstanceData(0);
            if (instanceData == NULL)
            {
                REX::WARN("Instance data in equip is null.");
            }

            auto instance = new RE::BGSObjectInstance(armor, instanceData);
            return equipManager->EquipObject(actor, *instance, 0, 1, armor->equipSlot, false, true, false, true, false);
        }
    }

    REX::ERROR("Couldn't find armor in actors inventory to equip.");
    return false;
}

bool ActorManager::ProcessHairStubs(RE::Actor* actor, const RE::BGSObjectInstance* armor, bool isUnequipEvent)
{
    auto setup = Setup::GetForms("headgear");
    if (setup.isEmpty)
    {
        return true;
    }

    bool isVisibleHelmetWorn = ActorManager::WornHasKeyword(actor, setup.keyword) &&
        !ActorManager::WornHasKeyword(actor, setup.keywordHidden);

    bool isEquipped = ActorManager::IsItemEquipped(actor, armor);

    if (!isUnequipEvent && !isEquipped)
    {
        // Skip broken events
        return false;
    }

    // REX::INFO(std::format("Analyze is visible: {0}, is unequip: {1}, is equipped: {2}, form id: {3}", isVisibleHelmetWorn, isUnequipEvent, isEquipped, armor.object->GetFormID()));

    auto armorHairTop = setup.armorHairTop;
    auto armorHairLong = setup.armorHairLong;
    auto armorHairBeard = setup.armorHairBeard;

    uint8_t anyChange = 0;

    if (!isVisibleHelmetWorn || !isEquipped)
    {
        anyChange += UnequipItem(actor, armorHairTop) ? 1 : 0;
        anyChange += UnequipItem(actor, armorHairLong) ? 1 : 0;
        anyChange += UnequipItem(actor, armorHairBeard) ? 1 : 0;

        if (anyChange > 0)
        {
            REX::INFO("Should updated unequipped items.");
            actor->HandleItemEquip(true);
        }

        return isUnequipEvent != isEquipped;
    }

    bool res = true;

    if (ActorManager::WornHasKeyword(actor, setup.keywordHairTop))
    {
        bool equipSuccessful = EquipItem(actor, armorHairTop);
        res = res && equipSuccessful;
        anyChange += equipSuccessful ? 1 : 0;
    }

    if (ActorManager::WornHasKeyword(actor, setup.keywordHairLong))
    {
        bool equipSuccessful = EquipItem(actor, armorHairLong);
        res = res && equipSuccessful;
        anyChange += equipSuccessful ? 1 : 0;
    }

    if (ActorManager::WornHasKeyword(actor, setup.keywordHairBeard))
    {
        bool equipSuccessful = EquipItem(actor, armorHairBeard);
        res = res && equipSuccessful;
        anyChange += equipSuccessful ? 1 : 0;
    }

    if (anyChange > 0)
    {
        REX::INFO("Should updated equipped items.");
        actor->HandleItemEquip(true);
    }

    return res && (isUnequipEvent != isEquipped);
}