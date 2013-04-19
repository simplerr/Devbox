#pragma once
#include <windows.h>
#include "Utility.h"
#include "Component.h"

class TransformComponent : public Component
{
public:
	TransformComponent();

	void Init();
	void PostInit();
	void Update(float dt);

	void SetPosition(XMFLOAT3 position);
	void SetRotation(XMFLOAT3 rotation);
	void SetScale(XMFLOAT3 scale);

	XMMATRIX GetWorldMatrix();
	XMFLOAT3 GetPosition();
	XMFLOAT3 GetRotation();
	XMFLOAT3 GetScale();

	const char* GetName() { return g_Name; }
	static const char* g_Name;
private:
	XMFLOAT3 mPosition;
	XMFLOAT3 mScale;
	XMFLOAT3 mRotation;
};