#pragma once

#include "typedefs.h"
#include "LuaPlus.h"

namespace GLib {
	class Graphics;
}

class Component
{
public:
	Component();
	virtual ~Component();

	// Override these.
	virtual void Init(LuaPlus::LuaObject initData) = 0;
	virtual const char* GetName() = 0;
	virtual void PostInit() {};
	virtual void Update(float dt) {};
	virtual void Draw(GLib::Graphics* pGraphics) {};

	void SetOwner(StrongActorPtr pOwner);
	void SetId(ComponentId id);

	ComponentId GetId();
protected:
	StrongActorPtr mOwner;
private:
	ComponentId mId;
};