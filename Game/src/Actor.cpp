#include "Actor.h"
#include "GlibStd.h"
#include "Component.h"
#include "LuaManager.h"

Actor::Actor(ActorId id)
{
	mId = id;
	mType = "Unknown";
}

Actor::~Actor()
{
	GLIB_LOG("Actor", std::string("Destroying Actor ") + to_string(mId));

	// If this assert fires, the actor was destroyed without calling Actor::Destroy().
	GLIB_ASSERT(mComponents.empty());  
}

void Actor::Init(LuaPlus::LuaObject luaObject, string actorName)
{
	//LuaManager::Get()->PrintTable(luaObject);
	mName = actorName;
}

void Actor::PostInit(void)
{

}

void Actor::Destroy(void)
{
	mComponents.clear();
}

void Actor::Update(float dt)
{
	for(ActorComponents::iterator iter = mComponents.begin(); iter != mComponents.end(); iter++)
	{
		iter->second->Update(dt);
	}
}

void Actor::Draw(GLib::Graphics* pGraphics)
{
	for(ActorComponents::iterator iter = mComponents.begin(); iter != mComponents.end(); iter++)
	{
		iter->second->Draw(pGraphics);
	}
}

void Actor::AddComponent(StrongComponentPtr pComponent)
{
	std::pair<ActorComponents::iterator, bool> success = mComponents.insert(std::make_pair(pComponent->GetName(), pComponent));
	GLIB_ASSERT(success.second);
}

const Actor::ActorComponents* Actor::GetComponents()
{
	return &mComponents; 
}

ActorId Actor::GetId()
{
	return mId;
}

string Actor::GetName()
{
	return mName;
}