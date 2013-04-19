#pragma once
#include "Utility.h"
#include "Component.h"

class BoundingBoxComponent : public Component
{
public:
	BoundingBoxComponent();

	void Init();
	void Draw(GLib::Graphics* pGraphics);
	
	void SetExtends(float extends);
	float GetExtends();

	const char* GetName() { return g_Name; }
	static const char* g_Name;
private:
	float mExtends;
};