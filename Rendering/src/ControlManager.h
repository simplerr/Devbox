#pragma once
#include <string>
#include <vector>
#include "d3dUtil.h"
using namespace std;

namespace GLib {
	class Input;
	class Graphics;
}

class LuaWrapper;
class BasicControl;

class ControlManager
{
public:
	ControlManager(string luaScript);
	virtual ~ControlManager();

	virtual void Update(GLib::Input* pInput, float dt);
	virtual void Draw(GLib::Graphics* pGraphics);

	void AddControl(BasicControl* pControl);
	void LoadLuaProperties();
	void DeactivateAllControls();
	void OnResize(float width, float height);
private:
	vector<BasicControl*> mControlList;
	LuaWrapper* mLuaWrapper;
	string mLuaScript;
};