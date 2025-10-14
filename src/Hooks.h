#pragma once

#include "pch.h"

namespace Hooks
{
	struct InitLoadGame
	{
		static void thunk(RE::Actor* aThis, RE::BGSLoadFormBuffer* buf);

		static inline REL::HookVFT Hook{ RE::Actor::VTABLE[0], 0x13, thunk };
	};

	struct ShouldBackgroundClone
	{
		static bool thunk(RE::Actor* aThis);

		static inline REL::HookVFT Hook{ RE::Actor::VTABLE[0], 0x89, thunk };
	};

	struct Revert
	{
		static void thunk(RE::Actor* aThis, RE::BGSLoadFormBuffer* buf);

		static inline REL::HookVFT Hook{ RE::Actor::VTABLE[0], 0x15, thunk };
	};

	struct LoadGame
	{
		static void thunk(RE::Actor* aThis, RE::BGSLoadFormBuffer* buf);

		static inline REL::HookVFT Hook{ RE::Actor::VTABLE[0], 0x12, thunk };
	};
}
