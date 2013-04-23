#pragma once

#include "Component.h"

namespace GLib {
	class StaticModel;
}


class StaticModelComponent : public Component
{
public:
	StaticModelComponent();

	void Init(LuaPlus::LuaObject initData);
	void Draw(GLib::Graphics* pGraphics);

	void SetAlpha(float alpha);

	const char* GetName() { return g_Name; }
	static const char* g_Name;
private:
	GLib::StaticModel* mModel;
};
	