#include "Logger.hpp"

namespace swizzle
{
	Logger& Logger::getLogger()
	{
		static Logger log;
		return log;
	}

	void Logger::addLogger(LoggerIfc* logger)
	{
		mLoggers.push_back(logger);
	}

	void Logger::logTrace(const char* message)
	{
		for (auto it : mLoggers)
		{
			it->logTrace(message);
		}
	}

	void Logger::logInfo(const char* message)
	{
		for (auto it: mLoggers)
		{
			it->logInfo(message);
		}
	}

	void Logger::logWarning(const char* message)
	{
		for (auto it : mLoggers)
		{
			it->logWarning(message);
		}
	}

	void Logger::logError(const char* message)
	{
		for (auto it : mLoggers)
		{
			it->logError(message);
		}
	}

	void Logger::logCritical(const char* message)
	{
		for (auto it : mLoggers)
		{
			it->logCritical(message);
		}
	}

}