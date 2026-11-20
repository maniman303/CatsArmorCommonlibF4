#include "HairHooks.h"
#include "Files.h"
#include "Setup.h"
#include "ActorManager.h"

namespace HairHooks
{
	// F4SE::Init() drains REL::FHookStore while the plugin loads, so every hook
	// must be constructed during static initialization. Taking their addresses
	// here forces the inline definitions to be emitted.
	[[maybe_unused]] const void* const g_hookAnchors[]{
		std::addressof(HeadPartVisibility::HookHair),
		std::addressof(HeadPartVisibility::HookHairLong),
		std::addressof(HeadPartVisibility::HookBeard),
		std::addressof(HeadPartHide::Hook),
	};

	bool IsHairSlot(const RE::TESRace* a_race, std::int32_t a_slot)
	{
		if (a_race == nullptr) {
			return false;
		}

		const auto hair = std::to_underlying(a_race->data.hairObject);

		return a_slot == hair ||
		       a_slot == hair + 1 ||
		       a_slot == std::to_underlying(a_race->data.beardObject);
	}

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
		if (a_actor == nullptr) {
			return a_engineHide;
		}

		if (a_slot != std::to_underlying(RE::BIPED_OBJECT::kHairTop) &&
		    a_slot != std::to_underlying(RE::BIPED_OBJECT::kHairLong) &&
		    a_slot != std::to_underlying(RE::BIPED_OBJECT::kBeard)) {
			return a_engineHide;
		}

		if (!Files::IsFilePresent() || !Setup::IsInitialized())
		{
			return a_engineHide;
		}

		auto setup = Setup::GetForms("headgear");
		if (setup.isEmpty)
		{
			return a_engineHide;
		}

		auto isHelmetKywd = ActorManager::SafeWornHasKeyword(a_actor, setup.keyword);
		auto isHiddenKywd = ActorManager::SafeWornHasKeyword(a_actor, setup.keywordHidden);

		if (isHelmetKywd == ActorManager::kError || isHiddenKywd == ActorManager::kError)
		{
			REX::ERROR("Could not safely check keywords for actor during headgear visibility processing.");
			return a_engineHide;
		}

		if (isHelmetKywd == ActorManager::kTrue && isHiddenKywd == ActorManager::kTrue)
		{
			REX::INFO(std::format(
				"HairSlot: [hide blocked] slot={} actor=[0x{:08X}] [{}]",
				a_slot,
				a_actor->GetFormID(),
				a_actor->GetDisplayFullName()));
			return false;
		}

		return a_engineHide;
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

	void HeadPartHide::thunk(RE::NiAVObject* a_face, std::int32_t** a_slot)
	{
		auto* actor = GetNodeActor(a_face);

		if (actor == nullptr || a_slot == nullptr || *a_slot == nullptr) {
			Hook(a_face, a_slot);
			return;
		}

		const std::int32_t slot = **a_slot;
		const auto*       npc = actor->GetNPC();
		const auto*       race = npc != nullptr ? npc->GetFormRace() : nullptr;

		if (IsHairSlot(race, slot) && !ShouldHide(actor, slot, true)) {
			REX::INFO(std::format(
				"HairSlot: [hide blocked] slot={} ({}) actor=[0x{:08X}]",
				slot,
				GetSlotName(race, slot),
				actor->GetFormID()));
			return;
		}

		Hook(a_face, a_slot);
	}
}
