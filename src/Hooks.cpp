#include "Hooks.h"
#include "PerkDistributor.h"

namespace Hooks
{
	void InitLoadGame::thunk(RE::Actor* aThis, RE::BGSLoadFormBuffer* buf)
	{
		InitLoadGame::Hook(aThis, buf);

		REX::INFO("Hook: InitLoadGame");

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

		PerkDistributor::TryProcessNpc(npc);
	}

	bool ShouldBackgroundClone::thunk(RE::Actor* aThis)
	{
		bool res = ShouldBackgroundClone::Hook(aThis);

		REX::INFO("Hook: ShouldBackgroundClone");
		
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

		PerkDistributor::TryProcessNpc(npc);

		return res;
	}

	void Revert::thunk(RE::Actor* aThis, RE::BGSLoadFormBuffer* buf)
	{
		Revert::Hook(aThis, buf);

		REX::INFO("Hook: Revert");

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

		PerkDistributor::TryProcessNpc(npc);
	}

	void LoadGame::thunk(RE::Actor* aThis, RE::BGSLoadFormBuffer* buf)
	{
		LoadGame::Hook(aThis, buf);

		REX::INFO("Hook: LoadGame");

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

		PerkDistributor::TryProcessNpc(npc);
	}
}
