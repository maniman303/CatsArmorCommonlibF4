#pragma once

#include "PCH.h"

namespace HairHooks
{
	// FUN_14064E480 (RVA 0x64E480) is the engine function that applies the
	// hide/show decision to a hair, hair long or beard head part: it walks the
	// head part geometry and toggles the subsegments tagged with 0x1E + slot.
	// Its only caller is FUN_140506460, which runs from the equip update and the
	// reference/head update, so the three calls below see every actor including
	// the player, in both directions.
	//
	// The thunk resolves the owning actor from the node's userData (walking the
	// parent chain) and replaces the engine's hide flag with the value returned
	// by ShouldHide(actor, slot, engineHide). Change ShouldHide to control what
	// is rendered; it receives the actor, so the decision can read the equipped
	// item for the slot from a_actor->biped->object[slot].parent.object.
	//
	// No editor IDs or node names are used anywhere.
	struct HeadPartVisibility
	{
		static void thunk(RE::NiAVObject* a_node, std::int32_t a_slot, bool a_hide);

		// The three FUN_14064E480 calls inside FUN_140506460: hair, hair long,
		// beard.
		static inline REL::THook HookHair{ REL::Offset{ 0x506733 }, 0, thunk };
		static inline REL::THook HookHairLong{ REL::Offset{ 0x5067A3 }, 0, thunk };
		static inline REL::THook HookBeard{ REL::Offset{ 0x506863 }, 0, thunk };
	};

	// Whether the given biped slot's head part should be hidden.
	bool ShouldHide(RE::Actor* a_actor, std::int32_t a_slot, bool a_engineHide);
}
