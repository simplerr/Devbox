#include "ActorFactory.h"
#include "TransformComponent.h"
#include "BoundingBoxComponent.h"
#include "Actor.h"

ActorFactory::ActorFactory(string actorsLuaFile)
{
	mLastActorId = INVALID_ACTOR_ID;
	mActorsLuaFile = actorsLuaFile;

	// Register component creation functions.
	mComponentFactory.Register<TransformComponent>(TransformComponent::g_Name);
	mComponentFactory.Register<BoundingBoxComponent>(BoundingBoxComponent::g_Name);
}

ActorFactory::~ActorFactory()
{
	
}

StrongActorPtr ActorFactory::CreateActor(string actorName)
{
	StrongActorPtr pActor(new Actor(GetNextActorId()));

	if(pActor)
		pActor->Init();

	return pActor;
}

StrongComponentPtr ActorFactory::CreateComponent(string componentName)
{
	// Use the component factory to create the component.
	StrongComponentPtr pComponent(mComponentFactory.Create(componentName));

	if(pComponent)
	{
		pComponent->Init();
	}
	else
	{
		GLIB_ERROR("Couldn't find Component named " + componentName);
		return StrongComponentPtr();
	}

	return pComponent;
}

void ActorFactory::AddComponentToActor(StrongActorPtr pActor, string componentName)
{
	StrongComponentPtr pComponent = CreateComponent(componentName);
	pActor->AddComponent(pComponent);
	pComponent->SetOwner(pActor);
}

ActorId ActorFactory::GetNextActorId()
{
	// Could be on 1 line? [NOTE]
	++mLastActorId;
	return mLastActorId;
}