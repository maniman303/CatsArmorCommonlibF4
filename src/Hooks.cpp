#include "Hooks.h"

namespace Hooks
{
	void InitLoadGame::thunk(RE::Actor* aThis, RE::BGSLoadFormBuffer* buf)
	{
		InitLoadGame::func(aThis, buf);

		auto npc = aThis->GetNPC();
		if (npc == NULL)
		{
			return;
		}

		auto niObj = aThis->GetFullyLoaded3D();
		if (niObj == NULL)
		{
			return;
		}

		// TODO: Update perk / spell
	}

	bool ShouldBackgroundClone::thunk(RE::Actor* aThis)
	{
		bool res = ShouldBackgroundClone::func(aThis);
		
		auto npc = aThis->GetNPC();
		if (npc == NULL)
		{
			return res;
		}

		auto niObj = aThis->GetFullyLoaded3D();
		if (niObj == NULL)
		{
			return res;
		}

		// TODO: Update perk / spell

		return res;
	}

	void Revert::thunk(RE::Actor* aThis, RE::BGSLoadFormBuffer* buf)
	{
		Revert::func(aThis, buf);

		auto npc = aThis->GetNPC();
		if (npc == NULL)
		{
			return;
		}

		auto niObj = aThis->GetFullyLoaded3D();
		if (niObj == NULL)
		{
			return;
		}

		// TODO: Update perk / spell
	}

	void LoadGame::thunk(RE::Actor* aThis, RE::BGSLoadFormBuffer* buf)
	{
		LoadGame::func(aThis, buf);

		auto npc = aThis->GetNPC();
		if (npc == NULL)
		{
			return;
		}

		auto niObj = aThis->GetFullyLoaded3D();
		if (niObj == NULL)
		{
			return;
		}

		// TODO: Update perk / spell
	}

	void Install()
	{
		Hooks::WriteVFunc<RE::Actor, Hooks::InitLoadGame>();
		Hooks::WriteVFunc<RE::Actor, Hooks::ShouldBackgroundClone>();
		Hooks::WriteVFunc<RE::Actor, Hooks::Revert>();
		Hooks::WriteVFunc<RE::Actor, Hooks::LoadGame>();
	}
}
