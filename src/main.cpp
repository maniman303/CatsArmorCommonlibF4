#include <json/json.h>
#include <iostream>
#include <fstream>
#include <bitset>
#include "FormUtil.h"
#include "Workaround.h"
#include "Setup.h"
#include "Hooks.h"
#include "Files.h"
#include "PapyrusUtil.h"
#include "ArmorProcessor.h"
#include "HeadgearProcessor.h"
#include "PerkDistributor.h"
#include "ActorEquipManagerListener.h"
#include "ExclusionManager.h"

bool InitModCore(const F4SE::QueryInterface* a_f4se)
{
	if (a_f4se->IsEditor()) {
		REX::ERROR("loaded in editor");
		return false;
	}

	const auto ver = a_f4se->RuntimeVersion();
	if (ver < F4SE::RUNTIME_1_11_159) {
		auto verString = ver.string();
		REX::ERROR(std::format("unsupported runtime v{}", verString));
		return false;
	}

	return true;
}

void OnMessage(F4SE::MessagingInterface::Message* message)
{
	if (message->type == F4SE::MessagingInterface::kGameDataReady)
	{
		if (!Files::VerifyPaths())
		{
			REX::ERROR("The main plugin directory does not exists.");
			return;
		}

		Files::PrepareDirectories();

		if (!Files::VerifyCatsPlugin())
		{
			REX::ERROR("Plugin 'Cats Armor.esl' is missing.");
			return;
		}

		if (!Setup::Initialize())
		{
			REX::ERROR("Incomplete setup.");
			return;
		}

		ExclusionManager::Initialize();

		ArmorProcessor::ProcessArmorFiles();
		HeadgearProcessor::ProcessHeadgearFiles();

		ActorEquipManagerListener::Register();

		PerkDistributor::ProcessMemoryActors();

		REX::INFO("Finished pre-game processing.");
	}
}

F4SE_PLUGIN_LOAD(const F4SE::LoadInterface* a_f4se)
{
	F4SE::Init(a_f4se, { .trampoline = true });

	InitModCore(a_f4se);
	
	F4SE::GetPapyrusInterface()->Register(PapyrusUtil::BindFunctions);

	F4SE::GetMessagingInterface()->RegisterListener(OnMessage);

	REX::INFO("Finished initialization.");

	return true;
}