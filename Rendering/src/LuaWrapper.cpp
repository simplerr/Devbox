#include "LuaWrapper.h"
#include "lua_Particle.h"

LuaWrapper::LuaWrapper(string luaScript)
{
	mScriptName = luaScript;
	mLuaState = nullptr;
	Reload();
}

LuaWrapper::~LuaWrapper()
{
	//lua_close(mLuaState);
}

void LuaWrapper::Reload()
{
	//if(mLuaState != nullptr)
	//	lua_close(mLuaState);

	mLuaState = lua_open();

	luaL_openlibs(mLuaState);
	tolua_open(mLuaState);

	//tolua_Particle_open(mLuaState);
	int error = luaL_dofile(mLuaState, mScriptName.c_str());
}

float LuaWrapper::GetNumber(string function)
{
	lua_getglobal(mLuaState, function.c_str());
	lua_pcall(mLuaState, 0, 1, 0);
	return lua_tonumber(mLuaState, -1);
}
//
//string LuaWrapper::GetString(string function)
//{
//	lua_getglobal(mLuaState, function.c_str());
//	lua_pcall(mLuaState, 0, 1, 0);
//	return string(lua_tostring(mLuaState, -1));
//}
//
//float LuaWrapper::GetTableNumber(string table, string element, int n)
//{
//	lua_getglobal(mLuaState, table.c_str());
//	lua_pushstring(mLuaState, element.c_str());
//	lua_gettable(mLuaState, -2);
//	return lua_tonumber(mLuaState, -1);
//}
//
//string LuaWrapper::GetTableString(string table, string element)
//{
//	lua_getglobal(mLuaState, table.c_str());
//	lua_pushstring(mLuaState, element.c_str());
//	lua_gettable(mLuaState, -2);
//	return string(lua_tostring(mLuaState, -1));
//}

lua_State* LuaWrapper::GetLua()
{
	return mLuaState;
}