#include "ActorEquipManagerListener.h"
#include "PerkDistributor.h"
#include "PapyrusUtil.h"

class ActorEquipManagerSink : public RE::BSTEventSink<RE::ActorEquipManagerEvent::Event>
{
public:
    static ActorEquipManagerSink* GetSingleton()
    {
        static ActorEquipManagerSink singleton;
        return std::addressof(singleton);
    }

private:
    RE::TESObjectARMO* TryGetHeadgear(RE::TESForm* item)
    {
        if (item == NULL)
        {
            return NULL;
        }

        if (!item->Is(RE::ENUM_FORM_ID::kARMO))
        {
            return NULL;
        }

        auto armor = item->As<RE::TESObjectARMO>();

        if ((armor->formFlags & 4) != 0)
		{
			return NULL;
		}

        uint32_t hairTopMask = 1;
		uint32_t hairLongMask = 2;
		uint32_t hairBeardMask = 1 << 18;
		uint32_t headbandMask = 1 << 16;

        uint32_t mask = hairTopMask | hairLongMask | hairBeardMask | headbandMask;

        auto bipedSlots = armor->bipedModelData.bipedObjectSlots;
        if ((bipedSlots & mask) == 0)
        {
            return NULL;
        }

        return armor;
    }

    void SendPapyrusEvent(RE::TESObjectREFR* arg)
	{
		struct PapyrusEventData
		{
			RE::BSScript::IVirtualMachine* vm;
			RE::TESObjectREFR* arg;
		};

		PapyrusEventData evntData;
		auto const papyrus = F4SE::GetPapyrusInterface();
		auto* vm = RE::GameVM::GetSingleton()->GetVM().get();

		evntData.vm = vm;
		evntData.arg = arg;

		papyrus->GetExternalEventRegistrations("HeadgearEquipEvent", &evntData, [](uint64_t handle, const char* scriptName, const char* callbackName, void* dataPtr) {
			PapyrusEventData* d = static_cast<PapyrusEventData*>(dataPtr);
			d->vm->DispatchMethodCall(handle, scriptName, callbackName, NULL);
		});
	}

    RE::BSEventNotifyControl ProcessEvent(const RE::ActorEquipManagerEvent::Event& aEvent, RE::BSTEventSource<RE::ActorEquipManagerEvent::Event>*) override
    {
        auto actor = aEvent.actorAffected;
        auto itemInstance = aEvent.itemAffected;

        if (actor == NULL || itemInstance == NULL)
        {
            return RE::BSEventNotifyControl::kContinue;
        }

        auto npc = actor->GetNPC();
        if (npc == NULL)
        {
            return RE::BSEventNotifyControl::kContinue;
        }

        auto item = TryGetHeadgear(itemInstance->object);
        if (item == NULL)
        {
            return RE::BSEventNotifyControl::kContinue;
        }

        if (!PerkDistributor::IsNpcValid(npc))
        {
            return RE::BSEventNotifyControl::kContinue;
        }

        REX::INFO("Send headgear event");

        SendPapyrusEvent(actor);

        return RE::BSEventNotifyControl::kContinue;
    }
};

void ActorEquipManagerListener::Register()
{
    auto *mgr = RE::ActorEquipManager::GetSingleton();

    if (mgr == NULL)
    {
        REX::WARN("ActorEquipManager is null.");

        return;
    }

    mgr->RegisterSink(ActorEquipManagerSink::GetSingleton());

    REX::INFO("ActorEquipManager event listener registered.");
}