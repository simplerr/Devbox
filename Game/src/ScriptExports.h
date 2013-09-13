#pragma once

#include "LuaPlus.h"
#include <string>

class ScriptExports
{
public:
	static void Register();

	// Input exports.
	static bool KeyPressed(const char* key);
	static bool KeyDown(const char* key);

	static LuaPlus::LuaObject GetCursorPosition();

	// Graphics exports.
	static void DrawString(const char* text, int x, int y, int size);
	static void DrawTexture(const char* texture, int x, int y, int width, int height);
	static void DrawAABB(float x, float y, float z, float extents, float alpha);

	// Camera exports.
	static void SetCameraPosition(float x, float y, float z);
	static void SetCameraDirection(float x, float y, float z);
	static void SetCameraTarget(float x, float y, float z);
	static void SetFogStart(float start);
	static void SetFogRange(float range);

	static LuaPlus::LuaObject GetCameraPosition();
	static LuaPlus::LuaObject GetCameraDirection();
	static LuaPlus::LuaObject GetCameraTarget();
	static LuaPlus::LuaObject GetCameraRight();
	
private:
	static int MapToKey(std::string str);
};