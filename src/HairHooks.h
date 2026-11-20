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
	// FUN_140360540 is a second, independent hide path: the biped pass
	// (FUN_14035E3B0) calls it to hide the hair/beard slots that share the worn
	// headgear's ARMA. HeadPartHide intercepts it so ShouldHide stays the single
	// source of truth.
	//
	// FUN_14064E570 is a third path for a head part's extraParts (hairline,
	// etc.); it takes a { face node, slot, hide } context by pointer, so
	// HeadPartExtra rewrites the hide flag in place before the original runs.
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

	struct HeadPartHide
	{
		static void thunk(RE::NiAVObject* a_face, std::int32_t** a_slot);

		// FUN_140360540 call inside FUN_14035E3B0.
		static inline REL::THook Hook{ REL::Offset{ 0x35E796 }, 0, thunk };
	};

	// Context FUN_14064E570 takes by pointer: the face node, the biped slot and
	// the hide flag it applies to the extra part's subsegments.
	struct HeadPartContext
	{
		RE::NiAVObject* face;   // 00
		std::int32_t    slot;   // 08
		bool            hide;   // 0C
	};
	static_assert(sizeof(HeadPartContext) == 0x10);
	static_assert(offsetof(HeadPartContext, face) == 0x00);
	static_assert(offsetof(HeadPartContext, slot) == 0x08);
	static_assert(offsetof(HeadPartContext, hide) == 0x0C);

	struct HeadPartExtra
	{
		static std::uintptr_t thunk(HeadPartContext* a_ctx, void* a_extraPart);

		// The three FUN_14064E570 calls inside FUN_140506460: the extra parts
		// (hairline, ...) of hair, hair long and beard.
		static inline REL::THook HookHair{ REL::Offset{ 0x506768 }, 0, thunk };
		static inline REL::THook HookHairLong{ REL::Offset{ 0x5067D8 }, 0, thunk };
		static inline REL::THook HookBeard{ REL::Offset{ 0x506898 }, 0, thunk };
	};
}
