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
		EventHandlerList()
			: mEventHandlers()
			, mBlockEvents(false)
		{ }

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
			if (!mBlockEvents)
			{
				for (const auto& it : mEventHandlers)
				{
					it->publishEvent(e);
				}
			}
		}

		void blockEvents(bool block)
		{
			mBlockEvents = block;
		}

	private:
		std::list<EventHandler<T>*> mEventHandlers;
		bool mBlockEvents;
	};

}
#endif