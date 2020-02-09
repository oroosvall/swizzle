#ifndef SW_LOGGING_HPP
#define SW_LOGGING_HPP

namespace swizzle
{
	class LoggerIfc
	{
	public:

		virtual void logTrace(const char* message) = 0;
		virtual void logInfo(const char* message) = 0;
		virtual void logWarning(const char* message) = 0;
		virtual void logError(const char* message) = 0;
		virtual void logCritical(const char* message) = 0;

	};
}

#endif