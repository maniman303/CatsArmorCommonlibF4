#include <json/json.h>
#include <json/value.h>
#include <vector>
#include "HeadgearProcessor.h"
#include "Files.h"
#include "Setup.h"
#include "FormUtil.h"
#include "Workaround.h"

namespace HeadgearProcessor
{
	static void SetArmorAddonBipedIndexes(RE::TESObjectARMA* addon, Setup::TypedSetup setup)
	{
		uint32_t hairTopMask = 1;
		uint32_t hairLongMask = 2;
		uint32_t hairBeardMask = 1 << 18;

		auto bipedSlots = addon->bipedModelData.bipedObjectSlots;

		bipedSlots = bipedSlots & ~hairTopMask;
		bipedSlots = bipedSlots & ~hairLongMask;
		bipedSlots = bipedSlots & ~hairBeardMask;
		
		if (bipedSlots == 0) {
			uint32_t newSlot = 1 << (setup.bipedIndex - 30);

			bipedSlots = addon->bipedModelData.bipedObjectSlots;
			bipedSlots = bipedSlots | newSlot;

			addon->bipedModelData.bipedObjectSlots = bipedSlots;
		}
	}

	static std::vector<RE::BGSKeyword*> SetArmorBipedIndexes(RE::TESObjectARMO* armor, Setup::TypedSetup setup)
	{
		std::vector<RE::BGSKeyword*> res;

		uint32_t hairTopMask = 1;
		uint32_t hairLongMask = 2;
		uint32_t headbandMask = 1 << 16;
		uint32_t hairBeardMask = 1 << 18;

		uint32_t newSlot = 1 << (setup.bipedIndex - 30);

		auto bipedSlots = armor->bipedModelData.bipedObjectSlots;

		if (bipedSlots & ~hairTopMask) {
			res.push_back(setup.keywordHairTop);
		}

		if (bipedSlots & ~hairLongMask) {
			res.push_back(setup.keywordHairLong);
		}

		if (bipedSlots & ~hairBeardMask) {
			res.push_back(setup.keywordHairBeard);
		}

		bipedSlots = bipedSlots & ~hairTopMask;
		bipedSlots = bipedSlots & ~hairLongMask;
		bipedSlots = bipedSlots & ~hairBeardMask;

		bipedSlots = bipedSlots | headbandMask;
		bipedSlots = bipedSlots | newSlot;

		armor->bipedModelData.bipedObjectSlots = bipedSlots;

		for (auto& ae : armor->modelArray) {
			SetArmorAddonBipedIndexes(ae.armorAddon, setup);
		}

		return res;
	}

	static void TrySetBaseIndex(RE::TESObjectARMO* armor)
	{
		if (armor == NULL) {
			return;
		}

		if (armor->armorData.index != 0) {
			return;
		}

		bool updateIndex = false;

		for (auto& model : armor->modelArray) {
			if (model.index == 0) {
				updateIndex = true;
				model.index = 1;
			}
		}

		if (updateIndex) {
			armor->armorData.index = 1;
		}
	}

	static void AddArmorAddon(RE::TESObjectARMO* armor, Setup::TypedSetup setup)
	{
		RE::TESObjectARMO::ArmorAddon addonEntry{};

		addonEntry.index = 303;
		addonEntry.armorAddon = setup.armorAddon;

		std::vector<RE::TESObjectARMO::ArmorAddon> addonList;

		addonList.push_back(addonEntry);

		for (auto& ae : armor->modelArray) {
			if (ae.index == 303) {
				REX::INFO(std::format("Form {0} already has armor addon with index 303.", FormUtil::GetHexFormId(armor->GetFormID())));

				return;
			}

			addonList.push_back(ae);
		}

		armor->modelArray.clear();

		for (auto& ae : addonList) {
			armor->modelArray.push_back(ae);
		}
	}

	static void ProcessHeadgearForm(RE::TESObjectARMO* armor, Setup::TypedSetup setup)
	{
		auto keywordsToAdd = SetArmorBipedIndexes(armor, setup);

		for (auto& keyword : keywordsToAdd) {
			armor->AddKeyword(keyword);
		}

		armor->AddKeyword(setup.keyword);

		//Workaround::AddAttachKeyword(armor, setup.attachSlot);
		armor->attachParents.AddKeyword(setup.attachSlot);

		TrySetBaseIndex(armor);

		AddArmorAddon(armor, setup);
	}

	static void ProcessHeadgearRecords(RE::TESForm* listBasic, Setup::TypedSetup setup)
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

			ProcessHeadgearForm(armor, setup);

			count++;
		}

		REX::INFO(std::format("Processed {0} headgear records.", count));
	}

	void ProcessHeadgearFiles()
	{
		auto setup = Setup::GetForms("headgear");

		if (setup.isEmpty || !setup.isEnabled) {
			REX::WARN("Headgear support is missing or turned off.");
			return;
		}

		auto headgearFiles = Files::GetPluginFiles("Headgear");

		for (auto& entry : headgearFiles) {
			auto path = entry.path();

			Json::Value modJson;
			std::ifstream modFile;

			try {
				modFile.open(path);

				modFile >> modJson;

				modFile.close();
			} catch (std::exception ex) {
				REX::ERROR(std::format("Invalid json '{0}'.", path.string()));
			}

			for (auto& formList : modJson) {
				auto forms = FormUtil::GetFormsFromJson(formList, RE::ENUM_FORM_ID::kFLST);

				for (auto& form : forms) {
					ProcessHeadgearRecords(form, setup);
				}
			}
		}
	}
}
