#pragma once

#include "pch.h"
#include "FunctionArgs.h"

namespace PapyrusUtil
{
	RE::BSFixedString StringReplace(std::monostate, RE::BSFixedString a_input, RE::BSFixedString a_toFind, RE::BSFixedString a_toReplace);

	bool BindFunctions(RE::BSScript::IVirtualMachine* vm);
}
