#include "PapyrusUtil.h"

namespace PapyrusUtil
{
	void AddItem(
		RE::TESObjectREFR* a_ref,
		RE::TESObjectREFR* a_form,
		std::int32_t a_count,
		bool a_silent)
	{
		PapyrusUtil::CallPapyrusFunctionOnForm(a_ref, "ObjectReference", "AddItem", a_form, a_count, a_silent);
	}

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
}
