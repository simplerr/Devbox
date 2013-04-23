#pragma once

#include "Component.h"
#include "Light.h"

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

	GLib::Material GetMaterial();

	const char* GetName() { return g_Name; }
	static const char* g_Name;
private:
	GLib::StaticModel* mModel;
};
	