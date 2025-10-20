#pragma once

#include "pch.h"
#include "FunctionArgs.h"

namespace PapyrusUtil
{
	std::vector<int> GetArmorBipedSlots(std::monostate, RE::TESObjectARMO* armor);

	void LogScript(std::monostate, RE::BSFixedString message);

	void Notification(std::monostate, RE::BSFixedString message);

	bool BindFunctions(RE::BSScript::IVirtualMachine* vm);
}
