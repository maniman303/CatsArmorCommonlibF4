#pragma once

#include "pch.h"

namespace Hooks
{
	struct InitLoadGame
	{
		static void thunk(RE::Actor* aThis, RE::BGSLoadFormBuffer* buf);

		static inline REL::Relocation<decltype(thunk)> func;
		static inline constexpr std::size_t idx{ 0x13 };
	};

	struct ShouldBackgroundClone
	{
		static bool thunk(RE::Actor* aThis);

		static inline REL::Relocation<decltype(thunk)> func;
		static inline constexpr std::size_t idx{ 0x89 };
	};

	struct Revert
	{
		static void thunk(RE::Actor* aThis, RE::BGSLoadFormBuffer* buf);

		static inline REL::Relocation<decltype(thunk)> func;
		static inline size_t idx{ 0x15 };
	};

	struct LoadGame
	{
		static void thunk(RE::Actor* aThis, RE::BGSLoadFormBuffer* buf);

		static inline REL::Relocation<decltype(thunk)> func;
		static inline size_t idx{ 0x12 };
	};

	void Install();

	template <class F, size_t index, class T>
	void WriteVFunc()
	{
		REL::Relocation<std::uintptr_t> vtbl{ F::VTABLE[index] };
		T::func = vtbl.write_vfunc(T::idx, T::thunk);
	}

	template <class F, class T>
	void WriteVFunc()
	{
		WriteVFunc<F, 0, T>();
	}
}
