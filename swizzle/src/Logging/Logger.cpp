#include "Logger.hpp"

#include <cstdarg>

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

	void Logger::logTrace(const char* message, ...)
	{
		va_list vl;
		va_start(vl, message);
		
		char buffer[1024];
		vsnprintf_s(buffer, sizeof(buffer), message, vl);
		va_end(vl);

		for (auto it : mLoggers)
		{
			it->logTrace(buffer);
		}
	}

	void Logger::logInfo(const char* message, ...)
	{
		va_list vl;
		va_start(vl, message);

		char buffer[1024];
		vsnprintf_s(buffer, sizeof(buffer), message, vl);
		va_end(vl);

		for (auto it: mLoggers)
		{
			it->logInfo(buffer);
		}
	}

	void Logger::logWarning(const char* message, ...)
	{
		va_list vl;
		va_start(vl, message);

		char buffer[1024];
		vsnprintf_s(buffer, sizeof(buffer), message, vl);
		va_end(vl);

		for (auto it : mLoggers)
		{
			it->logWarning(buffer);
		}
	}

	void Logger::logError(const char* message, ...)
	{
		va_list vl;
		va_start(vl, message);

		char buffer[1024];
		vsnprintf_s(buffer, sizeof(buffer), message, vl);
		va_end(vl);

		for (auto it : mLoggers)
		{
			it->logError(buffer);
		}
	}

	void Logger::logCritical(const char* message, ...)
	{
		va_list vl;
		va_start(vl, message);

		char buffer[1024];
		vsnprintf_s(buffer, sizeof(buffer), message, vl);
		va_end(vl);

		for (auto it : mLoggers)
		{
			it->logCritical(buffer);
		}
	}

}