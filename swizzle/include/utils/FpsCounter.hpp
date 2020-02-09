#ifndef FPS_COUNTER_HPP
#define FPS_COUNTER_HPP

#include <cstdint>

namespace utils
{
	class FpsCounter
	{
	public:
		FpsCounter();

		void tick(float dt);
		uint32_t getFps() const;

	private:

		uint32_t mFps;
		uint32_t mCount;

		float mTime;

	};
}

#endif