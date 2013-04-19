#pragma once
#include <string>
#include "GenericObjectFactory.h"
#include "typedefs.h"

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
	ActorFactory(string actorsLuaFile);
	virtual ~ActorFactory();

	StrongActorPtr CreateActor(string actorName);
	StrongComponentPtr CreateComponent(string componentName);
	void AddComponentToActor(StrongActorPtr pActor, string componentName);
private:
	ActorId GetNextActorId();

	string	mActorsLuaFile;
	ActorId mLastActorId;
protected:
	GenericObjectFactory<Component, string> mComponentFactory;
};