#ifndef EVENT_LIST_HPP
#define EVENT_LIST_HPP

#include <swizzle/events/Event.hpp>
#include <list>

namespace swizzle
{
	template<typename T>
	class EventHandlerList : EventHandler<T>
	{
	public:

		void addListener(EventHandler<T>* listener)
		{
			mEventHandlers.emplace_back(listener);
		}

		void removeListener(EventHandler<T>* listener)
		{
			mEventHandlers.remove(listener);
		}

		virtual void publishEvent(const T& e) override
		{
			for (const auto& it : mEventHandlers)
			{
				it->publishEvent(e);
			}
		}

	private:
		std::list<EventHandler<T>*> mEventHandlers;

	};

}
#endif