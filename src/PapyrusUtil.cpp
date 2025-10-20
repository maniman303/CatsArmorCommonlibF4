#include "PapyrusUtil.h"

namespace PapyrusUtil
{
	std::vector<int> GetArmorBipedSlots(std::monostate, RE::TESObjectARMO* armor)
	{
		std::vector<int> result;

		if (armor == NULL) {
			return result;
		}

		auto slotsBits = armor->bipedModelData.bipedObjectSlots;

		for (int i = 0; i < 44; i++) {
			if ((slotsBits & 1) > 0) {
				result.push_back(i);
			}

			slotsBits = slotsBits >> 1;
		}

		return result;
	}

	void LogScript(std::monostate, RE::BSFixedString message)
	{
		std::string text = message.c_str();

		REX::INFO(std::format("[Script] {0}", text));
	}

	void Notification(std::monostate, RE::BSFixedString message)
	{
		auto vm = RE::GameVM::GetSingleton()->GetVM().get();

		vm->DispatchStaticCall("Debug", "Notification", NULL, message);
	}

	bool BindFunctions(RE::BSScript::IVirtualMachine* vm)
	{
		vm->BindNativeMethod("CATS:ScriptExtender", "GetArmorBipedSlots", PapyrusUtil::GetArmorBipedSlots, true);
		vm->BindNativeMethod("CATS:ScriptExtender", "Trace", PapyrusUtil::LogScript, true);
		vm->BindNativeMethod("CATS:ScriptExtender", "Notification", PapyrusUtil::Notification);

		return true;
	}
}
