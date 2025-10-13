#pragma once

#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include "pch.h"

namespace Files
{
	std::filesystem::path GetRootPath();

	std::filesystem::path GetPluginPath();

	bool VerifyPaths();

	bool VerifyScatPlugin();

	std::vector<std::filesystem::directory_entry> GetPluginFiles(std::string path);

	void PrepareDirectories();
}
