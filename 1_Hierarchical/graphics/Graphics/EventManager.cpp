#include "EventManager.h"
#include "ResourceManager.h"
#include "GameObject.h"

extern ResourceManager* resourceManager;

EventManager::EventManager()
{
}


EventManager::~EventManager()
{
}

void EventManager::RegisterEvent()
{

}

void EventManager::Update()
{
	
}

void EventManager::BroadCastEvent(const Event * const evnt) const
{
	for (auto i : resourceManager->Get2DGameObjects())
	{
		if (i->IsAlive) // or i->NeedsDeletion
			i->HandleEvent(evnt);
	}
	for (auto i : resourceManager->Get3DGameObjects())
	{
		if (i->IsAlive) // or i->NeedsDeletion
			i->HandleEvent(evnt);
	}
	delete evnt;
}
