#ifndef PROFILE_HPP
#define PROFILE_HPP

#include <cstdint>

#include <unordered_map>
#include <atomic>
#include <fstream>

#include <swizzle/Platform.hpp>

namespace swizzle
{

	struct timestamp
	{
		char* mFile;
		char* mFuncSig;
		uint32_t mThreadId;
		uint32_t mLineNr;
		uint64_t mStartTime;
		uint64_t mEndTime;
	};

	class Profiler
	{
	private:
		struct NameStore
		{
			uint32_t mNameIndex;
			const char* mName;
		};

	public:
		Profiler()
			: mNameIndexCounter(0U)
			, mNameCache()
			, mNameCacheSize(0U)
			, mRecording(true)
		{}

		void startProfiler(const char* name, const char* filePath)
		{
			std::string dataFile(filePath);
			dataFile += name;
			dataFile += ".dat";
			std::string nameFile(filePath);
			nameFile += name;
			nameFile += ".nam";

			mDataFile.open(dataFile, std::ios::binary);
			mNameFile.open(nameFile, std::ios::binary);
		}

		void startRecording()
		{
			mRecording = true;
		}

		void stopRecording()
		{
			mRecording = false;
		}

		void endProfiler()
		{
			mDataFile.close();
			mNameFile.close();
		}
		
		void timeStamp(timestamp& ts)
		{
			if (mRecording)
			{
				const uint32_t fileIndex = getNameIndex(ts.mFile);
				const uint32_t funcIndex = getNameIndex(ts.mFuncSig);
				const uint32_t lineNumber = ts.mLineNr;

				mDataFile.write((char*)&fileIndex, sizeof(uint32_t));
				mDataFile.write((char*)&funcIndex, sizeof(uint32_t));
				mDataFile.write((char*)&lineNumber, sizeof(uint32_t));
				mDataFile.write((char*)&ts.mThreadId, sizeof(uint32_t));
				mDataFile.write((char*)&ts.mStartTime, sizeof(uint64_t));
				mDataFile.write((char*)&ts.mEndTime, sizeof(uint64_t));

				mDataFile.flush();
			}
		}

		static Profiler& get()
		{
			static Profiler instance;
			return instance;
		}

	private:

		void optimizedSave()
		{

		}

		void storeAddedName(uint32_t index, const char* str)
		{
			mNameFile.write((char*)&index, sizeof(uint32_t));
			uint64_t len = strlen(str);
			mNameFile.write((char*)&len, sizeof(uint64_t));
			mNameFile.write(str, len);
			mNameFile.flush();
		}

		uint32_t getNameIndex(const char* str)
		{
			uint32_t index = 0;
			auto found = mNameCache.find(str);
			if (found == mNameCache.end())
			{
				index = mNameIndexCounter++;
				mNameCache[str] = index;
				storeAddedName(index, str);
			}
			else
			{
				index = found->second;
			}

			return index;
		}

		std::atomic_uint32_t mNameIndexCounter;
		std::unordered_map<const char*, uint32_t> mNameCache; // make sure this map is thread safe
		size_t mNameCacheSize;

		std::ofstream mDataFile;
		std::ofstream mNameFile;
		bool mRecording;

	};

	class Timer
	{
	public:
		Timer(const char* file, const char* funcSig, const uint32_t lineNr)
			: ts()
		{
			ts.mFile = (char*)file;
			ts.mFuncSig = (char*)funcSig;
			ts.mLineNr = lineNr;
			ts.mThreadId = 0;
			ts.mStartTime = swizzle::platform::getPlatformCurrentThreadId();
		}
		~Timer()
		{
			ts.mEndTime = swizzle::platform::getPlatformTimeStamp();
			Profiler::get().timeStamp(ts);
		}
	private:
		timestamp ts;
	};

}

#ifdef OTP_PROFILING
#define START_PROFILER(name, filePath) swizzle::Profiler::get().startProfiler(name, filePath)
#define PROFILE_FUNCTION() Timer timer##__LINE__(__FILE__, __FUNCSIG__, __LINE__)
#define PROFILE_SCOPE(name) Timer timer##__LINE__(__FILE__, name, __LINE__)
#define END_PROFILER() swizzle::Profiler::get().endProfiler()
#else
#define START_PROFILER(name, filePath)
#define PROFILE_FUNCTION()
#define PROFILE_SCOPE(name)
#define END_PROFILER()
#endif

#endif