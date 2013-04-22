#pragma once

#include "Component.h"

class ScriptComponent : public Component
{
public:
	ScriptComponent();

	void Init(LuaPlus::LuaObject initData);
	void PostInit();

	void Update(float dt);
	void Draw(GLib::Graphics* pGraphics);

	void CreateScriptObject();
	void RegisterScriptFunctions();

	void SetPosition(float x, float y, float z);
	void SetRotation(float x, float y, float z);
	void SetScale(float x, float y, float z);

	LuaPlus::LuaObject GetPosition();
	LuaPlus::LuaObject GetRotation();
	LuaPlus::LuaObject GetScale();

	const char* GetName() { return g_Name; }
	static const char* g_Name;
private:
	int GetActorId();

private:
	LuaPlus::LuaObject mLuaObject;	// The Lua reference to this component.
	LuaPlus::LuaObject mLuaOnCreated;
	LuaPlus::LuaObject mLuaOnUpdate;
	LuaPlus::LuaObject mLuaOnDraw;
};