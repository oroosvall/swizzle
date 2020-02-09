#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <swizzle/Logging.hpp>

#include <vector>

namespace swizzle
{

	class Logger
	{
	public:
		static Logger& getLogger();

		void addLogger(LoggerIfc* logger);

		void logTrace(const char* message);
		void logInfo(const char* message);
		void logWarning(const char* message);
		void logError(const char* message);
		void logCritical(const char* message);

	private:

		std::vector<LoggerIfc*> mLoggers;

	};


#define LOG_STR(a) LOG_STR2(a)
#define LOG_STR2(a) #a

#define LOG_TRACE(msg) Logger::getLogger().logTrace(__FILE__ ":" LOG_STR(__LINE__) "\n" msg)
#define LOG_INFO(msg) Logger::getLogger().logInfo(__FILE__ ":" LOG_STR(__LINE__) "\n" msg)
#define LOG_WARNING(msg) Logger::getLogger().logWarning(__FILE__ ":" LOG_STR(__LINE__) "\n" msg)
#define LOG_ERROR(msg) Logger::getLogger().logError(__FILE__ ":" LOG_STR(__LINE__) "\n" msg)
#define LOG_CRITICAL(msg) Logger::getLogger().logCritical(__FILE__ ":" LOG_STR(__LINE__) "\n" msg)

}

#endif