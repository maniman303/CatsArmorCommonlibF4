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
#include "IndexProcessor.h"
#include "ArmorProcessor.h"
#include "HeadgearProcessor.h"

bool InitModCore(const F4SE::QueryInterface* a_f4se)
{
	if (a_f4se->IsEditor()) {
		REX::ERROR("loaded in editor");
		return false;
	}

	const auto ver = a_f4se->RuntimeVersion();
	if (ver < F4SE::RUNTIME_1_10_980) {
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

		if (!Files::VerifyScatPlugin())
		{
			REX::ERROR("Plugin 'Scat Armor.esl' is missing.");
			return;
		}

		if (!Setup::Initialize())
		{
			REX::ERROR("Incomplete setup.");
			return;
		}

		// IndexProcessor::ProcessIndexLists();
		ArmorProcessor::ProcessArmorFiles();
		HeadgearProcessor::ProcessHeadgearFiles();

		REX::INFO("Finished pre-game processing.");
	}
}

bool BindPapyrusFunctions(RE::BSScript::IVirtualMachine* vm)
{
	vm->BindNativeMethod("SCAT:ScriptExtender", "GetArmorBipedSlots", PapyrusUtil::GetArmorBipedSlots, true);
	vm->BindNativeMethod("SCAT:ScriptExtender", "Trace", PapyrusUtil::LogScript, true);

	return true;
}

F4SE_PLUGIN_LOAD(const F4SE::LoadInterface* a_f4se)
{
	F4SE::Init(a_f4se, F4SE::InitInfo());

	InitModCore(a_f4se);

	F4SE::GetPapyrusInterface()->Register(BindPapyrusFunctions);

	F4SE::GetMessagingInterface()->RegisterListener(OnMessage);

	REX::INFO("Finished initialization.");

	return true;
}

// extern "C" DLLEXPORT constinit auto F4SEPlugin_Version = []() noexcept {
// 	F4SE::PluginVersionData data{};

// 	data.PluginVersion(REL::Version(1, 0, 0));
// 	data.PluginName("ScatArmorPlugin");
// 	data.AuthorName("maniman303");
// 	data.UsesAddressLibrary(true);
// 	data.UsesSigScanning(false);
// 	data.IsLayoutDependent(true);
// 	data.HasNoStructUse(false);
// 	data.CompatibleVersions({ F4SE::RUNTIME_LATEST });

// 	return data;
// }();