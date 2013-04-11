#pragma once
#include <string>

extern "C" {
#include "tolua++.h"
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
};

using namespace std;

class LuaWrapper
{
public:
	LuaWrapper(string luaScript);
	~LuaWrapper();

	void Reload();

	/*float GetTableNumber(string table, string element, int n = 0);
	string  GetTableString(string table, string element);*/
	float GetNumber(string function);
	//string GetString(string function);*/
	lua_State* GetLua();

	// get function
	template<typename T>
	T Get(string varname) {
		char temp[128];
		memset(temp, 0, sizeof(temp));
		int i=0;
		int j=0;
		int n=0;
		while (varname[i] != '\0') {
			char c = varname[i];
			if (c == '.') {
				if (n == 0)
					lua_getglobal(mLuaState, temp);
				else
					lua_getfield(mLuaState, -1, temp);
				++n;
				memset(temp, 0, sizeof(temp));
				j = 0;

				if (lua_isnil(mLuaState, -1))
					return 0;
			}
			else {
				temp[j] = c;
				++j;
			}
			++i;
		}
		if (n == 0)
			lua_getglobal(mLuaState, temp);
		else
			lua_getfield(mLuaState, -1, temp);
		return lua_get<T>();
	}

	// Generic get
	template<typename T>
	T lua_get() {
		return 0;
	}

	// Specializations
	template <> float lua_get<float>() {
		return lua_tonumber(mLuaState, -1);
	}
	template <> double lua_get<double>() {
		return lua_tonumber(mLuaState, -1);
	}
	template <> bool lua_get<bool>() {
		return (bool)lua_toboolean(mLuaState, -1);
	}
	template <> int lua_get<int>() {
		return lua_tointeger(mLuaState, -1);
	}
	template <> unsigned int lua_get<unsigned int>() {
		return (unsigned int)lua_tonumber(mLuaState, -1);
	}
	template <> string lua_get<string>() {
		return lua_tostring(mLuaState, -1);
	}
private:
	lua_State* mLuaState;
	string mScriptName;
};