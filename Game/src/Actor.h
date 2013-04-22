#pragma once
#include <memory>
#include <map>
#include "typedefs.h"
#include "LuaPlus.h"
using namespace std;

class Component;

namespace GLib {
	class Graphics;
}

class Actor
{
public:
	typedef map<ComponentName, StrongComponentPtr> ActorComponents;
public:
	explicit Actor(ActorId id);
	~Actor();

	void Init(LuaPlus::LuaObject luaObject, string actorName);
	void PostInit(void);
	void Destroy(void);
	void Update(float dt);
	void Draw(GLib::Graphics* pGraphics);
	void AddComponent(StrongComponentPtr pComponent);

	const ActorComponents* GetComponents();
	ActorId GetId();
	string GetName();

	template <class ComponentType>
	weak_ptr<ComponentType> GetComponent(ComponentId id);

	template <class ComponentType>
	weak_ptr<ComponentType> GetComponent(const char *name);

private:
	ActorComponents mComponents;
	ActorType mType;
	ActorId mId;
	string mName;
private:
	// Script objects, should probably not be here
};

/******************************************************************************************//**
* Template function for retrieving components
*********************************************************************************************/ 

template <class ComponentType>
weak_ptr<ComponentType> Actor::GetComponent(ComponentId id)
{
	ActorComponents::iterator findIt = mComponents.find(id);
	if (findIt != mComponents.end())
	{
		StrongComponentPtr pBase(findIt->second);
		shared_ptr<ComponentType> pSub(static_pointer_cast<ComponentType>(pBase));  // cast to subclass version of the pointer
		weak_ptr<ComponentType> pWeakSub(pSub);  // convert strong pointer to weak pointer
		return pWeakSub;  // return the weak pointer
	}
	else
	{
		return weak_ptr<ComponentType>();
	}
}

template <class ComponentType>
weak_ptr<ComponentType> Actor::GetComponent(const char *name)
{
	ActorComponents::iterator findIt = mComponents.find(name);
	if (findIt != mComponents.end())
	{
		StrongComponentPtr pBase(findIt->second);
		shared_ptr<ComponentType> pSub(static_pointer_cast<ComponentType>(pBase));  // cast to subclass version of the pointer
		weak_ptr<ComponentType> pWeakSub(pSub);  // convert strong pointer to weak pointer
		return pWeakSub;  // return the weak pointer
	}
	else
	{
		return weak_ptr<ComponentType>();
	}
}