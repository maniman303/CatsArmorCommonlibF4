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
		std::addressof(HeadPartExtra::HookHair),
		std::addressof(HeadPartExtra::HookHairLong),
		std::addressof(HeadPartExtra::HookBeard),
	};

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
			// REX::INFO(std::format(
			// 	"HairSlot: [hide blocked] slot={} actor=[0x{:08X}] [{}]",
			// 	a_slot,
			// 	a_actor->GetFormID(),
			// 	a_actor->GetDisplayFullName()));
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

		if (!ShouldHide(actor, slot, true)) {
			// REX::INFO(std::format(
			// 	"HairSlot: [hide blocked] slot={} actor=[0x{:08X}] [{}]",
			// 	slot,
			// 	actor->GetFormID(),
			// 	actor->GetDisplayFullName()));

			return;
		}

		Hook(a_face, a_slot);
	}

	bool IsHairOrBeardExtraPart(RE::Actor* a_actor, RE::BGSHeadPart* a_extraPart)
	{
		if (a_actor == nullptr || a_extraPart == nullptr) {
			return false;
		}

		const auto* npc = a_actor->GetNPC();
		if (npc == nullptr) {
			return false;
		}

		for (auto* part : npc->GetHeadParts()) {
			if (part == nullptr ||
			    (part->type != RE::BGSHeadPart::HeadPartType::kHair &&
			     part->type != RE::BGSHeadPart::HeadPartType::kFacialHair)) {
				continue;
			}

			for (auto* extra : part->extraParts) {
				if (extra == a_extraPart) {
					return true;
				}
			}
		}

		return false;
	}

	std::uintptr_t HeadPartExtra::thunk(HeadPartContext* a_ctx, RE::BGSHeadPart* a_extraPart)
	{
		if (a_ctx == nullptr) {
			return HookHair(a_ctx, a_extraPart);
		}

		auto* actor = GetNodeActor(a_ctx->face);
		if (actor == nullptr) {
			return HookHair(a_ctx, a_extraPart);
		}

		if (IsHairOrBeardExtraPart(actor, a_extraPart)) {
			a_ctx->hide = ShouldHide(actor, a_ctx->slot, a_ctx->hide);
		}

		return HookHair(a_ctx, a_extraPart);
	}
}
