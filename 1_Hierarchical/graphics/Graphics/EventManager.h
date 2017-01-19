#pragma once
#include "Event.h"

#include <list>

class EventManager
{
private:
	std::list<Event*> events;

public:
	EventManager();
	~EventManager();

	void RegisterEvent();
	void Update();
	void BroadCastEvent(const Event* const evnt) const;

	template<class T>
	T* const GenerateEvent()
	{
		T* evt = new T;
		return evt;
	}

};

