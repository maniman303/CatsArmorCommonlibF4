#include "HairHooks.h"

namespace HairHooks
{
	// F4SE::Init() drains REL::FHookStore while the plugin loads, so every hook
	// must be constructed during static initialization. Taking their addresses
	// here forces the inline definitions to be emitted.
	[[maybe_unused]] const void* const g_hookAnchors[]{
		std::addressof(HeadPartVisibility::HookHair),
		std::addressof(HeadPartVisibility::HookHairLong),
		std::addressof(HeadPartVisibility::HookBeard),
	};

	const char* GetSlotName(const RE::TESRace* a_race, std::int32_t a_slot)
	{
		if (a_race == nullptr) {
			return "hair";
		}

		const auto hair = std::to_underlying(a_race->data.hairObject);

		if (a_slot == std::to_underlying(a_race->data.beardObject)) {
			return "beard";
		}

		if (a_slot == hair + 1) {
			return "hair long";
		}

		return "hair";
	}

	void LogSlot(RE::Actor* a_actor, std::int32_t a_slot, bool a_hide)
	{
		if (a_actor == nullptr || a_slot < 0 || a_slot >= std::to_underlying(RE::BIPED_OBJECT::kEditorTotal)) {
			return;
		}

		const auto* npc = a_actor->GetNPC();
		const auto* race = npc != nullptr ? npc->GetFormRace() : nullptr;
		const auto* biped = a_actor->biped.get();
		const auto* item = biped != nullptr ? biped->object[a_slot].parent.object : nullptr;
		const auto* arma = biped != nullptr ? biped->object[a_slot].armorAddon : nullptr;

		REX::INFO(std::format(
			"HairSlot: slot={} ({}) state={} actor=[0x{:08X}] item=[0x{:08X}] arma=[0x{:08X}]",
			a_slot,
			GetSlotName(race, a_slot),
			a_hide ? "hidden" : "shown",
			a_actor->GetFormID(),
			item != nullptr ? item->GetFormID() : 0,
			arma != nullptr ? arma->GetFormID() : 0));
	}

	RE::Actor* GetNodeActor(RE::NiAVObject* a_node)
	{
		if (a_node == nullptr) {
			return nullptr;
		}

		for (auto* iter = a_node; iter != nullptr; iter = iter->parent) {
			if (iter->userData == 0) {
				continue;
			}

			auto* refr = reinterpret_cast<RE::TESObjectREFR*>(iter->userData);
			if (refr->Is<RE::Actor>()) {
				return refr->As<RE::Actor>();
			}
		}

		return nullptr;
	}

	bool ShouldHide(RE::Actor* a_actor, std::int32_t a_slot, bool a_engineHide)
	{
		// Default: pass through the engine's own decision.
		//
		// To force hair/beard always rendered, return false. To force it hidden,
		// return true. The equipped item for the slot is available for the
		// decision as:
		//     a_actor->biped.get()->object[a_slot].parent.object
		(void)a_actor;
		(void)a_slot;
		(void)a_engineHide;
		return false;
	}

	void HeadPartVisibility::thunk(RE::NiAVObject* a_node, std::int32_t a_slot, bool a_hide)
	{
		auto* actor = GetNodeActor(a_node);
		if (actor == nullptr) {
			HookHair(a_node, a_slot, a_hide);
			return;
		}

		const bool hide = ShouldHide(actor, a_slot, a_hide);
		LogSlot(actor, a_slot, hide);

		// Both detour calls to the same function, so HookHair's trampoline
		// reaches the original regardless of which site invoked us.
		HookHair(a_node, a_slot, hide);
	}
}
