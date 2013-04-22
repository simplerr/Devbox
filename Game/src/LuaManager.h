#pragma once

#include "LuaPlus.h"
#include <string>
#include "GlibStd.h"
#include <xnamath.h>

class LuaManager
{
public:
	static bool Create();
	static void Destroy();
	static LuaManager* Get();

	bool Init();
	void ExecuteFile(const char* path);
	void ExecuteString(const char* str);

	void SetError(int errorNum);

	LuaPlus::LuaObject GetGlobalVars();
	LuaPlus::LuaState* GetLuaState() const;

	// Debug functions
	void PrintTable(LuaPlus::LuaObject table, bool recursive = false);

	void ConvertVec3ToTable(const XMFLOAT3& vec, LuaPlus::LuaObject& outLuaTable) const;
private:
	// Singleton pattern.
	LuaManager();
	virtual ~LuaManager();

private:
	static LuaManager* mSingleton;
	LuaPlus::LuaState* mLuaState;
	std::string mLastError;
};