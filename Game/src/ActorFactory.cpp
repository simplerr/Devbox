#include "ActorFactory.h"
#include "TransformComponent.h"
#include "BoundingBoxComponent.h"
#include "ScriptComponent.h"
#include "StaticModelComponent.h"
#include "AnimatedModelComponent.h"
#include "Actor.h"
#include "LuaManager.h"
#include "GlibStd.h"

ActorFactory::ActorFactory()
{
	mLastActorId = INVALID_ACTOR_ID;

	// Register component creation functions.
	mComponentFactory.Register<TransformComponent>(TransformComponent::g_Name);
	mComponentFactory.Register<BoundingBoxComponent>(BoundingBoxComponent::g_Name);
	mComponentFactory.Register<ScriptComponent>(ScriptComponent::g_Name);
	mComponentFactory.Register<StaticModelComponent>(StaticModelComponent::g_Name);
	mComponentFactory.Register<AnimatedModelComponent>(AnimatedModelComponent::g_Name);
}

ActorFactory::~ActorFactory()
{
	
}

StrongActorPtr ActorFactory::CreateActor(const char* actorName)
{
	LuaPlus::LuaObject luaActor = LuaManager::Get()->GetLuaState()->GetGlobal(actorName);

	if(luaActor.IsNil())
		return nullptr;

	StrongActorPtr pActor(new Actor(GetNextActorId()));

	if(pActor)
		pActor->Init(luaActor, actorName);

	// Add all components.
	for(LuaPlus::LuaTableIterator iter(luaActor); iter; iter.Next())
	{
		LuaPlus::LuaObject key = iter.GetKey();
		LuaPlus::LuaObject componentData = iter.GetValue();

		if(!componentData.IsTable() || string(key.ToString()) == "__index")
		{
			//GLIB_LOG("ActorFactory", string("invalid component ") + key.ToString());
			continue;
		}

		GLIB_LOG("ActorFactory", string("added ") + key.ToString() + " to " + actorName);

		StrongComponentPtr component = CreateComponent(key.ToString(), componentData);
		pActor->AddComponent(component);
		component->SetOwner(pActor);
		component->Init(componentData);
	}

	// Loops over and PostInit() all components.
	pActor->PostInit();

	return pActor;
}

StrongComponentPtr ActorFactory::CreateComponent(string componentName, LuaPlus::LuaObject data)
{
	// Use the component factory to create the component.
	StrongComponentPtr pComponent(mComponentFactory.Create(componentName));

	if(!pComponent)
	{
		GLIB_ERROR("Couldn't find Component named " + componentName);
		return StrongComponentPtr();
	}

	return pComponent;
}

void ActorFactory::AddComponentToActor(StrongActorPtr pActor, string componentName)
{
	/*StrongComponentPtr pComponent = CreateComponent(componentName);
	pActor->AddComponent(pComponent);
	pComponent->SetOwner(pActor);*/
}

ActorId ActorFactory::GetNextActorId()
{
	// Could be on 1 line? [NOTE]
	++mLastActorId;
	return mLastActorId;
}