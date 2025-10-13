#pragma once

#include <stdlib.h>
#include <string>

namespace MakoLogger
{
	void init(std::string filePath, std::string header);

	void info(std::string message);

	void error(std::string message);
}