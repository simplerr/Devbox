#pragma once
#include <string>
#include "GenericObjectFactory.h"
#include "typedefs.h"
#include "LuaPlus.h"

using namespace std;

class Component;

/******************************************************************************************//**
* Handles the creation of Actors.
* Actors can only be created from an ActorFactory.
* This class should be derived to allow creation of game specific Actors.
*********************************************************************************************/
class ActorFactory
{
public:
	ActorFactory();
	virtual ~ActorFactory();

	StrongActorPtr CreateActor(const char* actorName);
	StrongComponentPtr CreateComponent(string componentName, LuaPlus::LuaObject data);
	void AddComponentToActor(StrongActorPtr pActor, string componentName);

private:
	ActorId GetNextActorId();
	ActorId mLastActorId;
protected:
	GenericObjectFactory<Component, string> mComponentFactory;
};