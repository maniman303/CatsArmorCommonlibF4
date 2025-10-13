#include <fstream>
#include <iostream>
#include <iomanip>
#include <time.h>
#include <mutex>
#include "MakoLogger.h"

namespace MakoLogger
{
	static std::string logPath;
	static bool isInitialized = false;
	static std::mutex mutex;

	void init(std::string filePath, std::string header)
	{
		logPath = filePath;
		std::ofstream logFile(logPath, std::ios::out);

		logFile << header << std::endl;

		logFile.close();

		isInitialized = true;
	}

	void info(std::string message)
	{
		if (!isInitialized) {
			return;
		}

		mutex.lock();

		auto t = std::time(NULL);
		struct tm tm;
		localtime_s(&tm, &t);

		std::ofstream logFile(logPath, std::ios::out | std::ios::app);

		logFile << std::put_time(&tm, "[%d-%m-%Y %H:%M:%S][info] ") << message << std::endl;

		logFile.close();

		mutex.unlock();
	}

	void error(std::string message)
	{
		if (!isInitialized) {
			return;
		}

		mutex.lock();

		auto t = std::time(NULL);
		struct tm tm;
		localtime_s(&tm, &t);

		std::ofstream logFile(logPath, std::ios::out | std::ios::app);

		logFile << std::put_time(&tm, "[%d-%m-%Y %H:%M:%S][error] ") << message << std::endl;

		logFile.close();

		mutex.unlock();
	}
}
