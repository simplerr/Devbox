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
private:
	static int MapToKey(std::string str);
};