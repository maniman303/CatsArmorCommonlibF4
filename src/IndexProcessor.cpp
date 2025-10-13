#include <json/json.h>
#include "IndexProcessor.h"
#include "Files.h"
#include "FormUtil.h"

namespace IndexProcessor
{
	static void ModArmorBaseAddonIndex(RE::TESObjectARMO* armor, uint16_t index)
	{
		if (armor == NULL) {
			return;
		}

		if (armor->armorData.index == index) {
			return;
		}

		armor->armorData.index = index;

		for (auto& model : armor->modelArray) {
			if (model.index == 0) {
				model.index = index;
			}
		}
	}

	static void ProcessIndexRecords(RE::TESForm* listBasic)
	{
		if (listBasic == NULL || listBasic->GetFormType() != RE::ENUM_FORM_ID::kFLST) {
			REX::INFO("Invalid index form list.");
			return;
		}

		auto list = listBasic->As<RE::BGSListForm>();

		int count = 0;

		for (auto& form : list->arrayOfForms) {
			if (form == NULL) {
				continue;
			}

			if (form->GetFormType() != RE::ENUM_FORM_ID::kARMO) {
				continue;
			}

			auto armor = form->As<RE::TESObjectARMO>();

			ModArmorBaseAddonIndex(armor, 1);

			count++;
		}

		REX::INFO(std::format("Updated indexes of {0} records.", count));
	}

	void ProcessIndexLists()
	{
		auto pluginFiles = Files::GetPluginFiles("Indexes");

		REX::INFO(std::format("Retrieved {0} index files to process.", pluginFiles.size()));

		for (auto& entry : pluginFiles) {
			auto path = entry.path();

			Json::Value modJson;
			std::ifstream modFile;

			try {
				modFile.open(path);

				modFile >> modJson;

				modFile.close();
			} catch (std::exception ex) {
				REX::INFO(std::format("Invalid json '{0}'.", path.string()));
			}

			for (auto& formList : modJson) {
				auto forms = FormUtil::GetFormsFromJson(formList, RE::ENUM_FORM_ID::kFLST);

				for (auto& form : forms) {
					ProcessIndexRecords(form);
				}
			}
		}
	}
}
