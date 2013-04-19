#include "BoundingBoxComponent.h"
#include "TransformComponent.h"
#include "Graphics.h"
#include "Actor.h"

const char* BoundingBoxComponent::g_Name = "BoundingBoxComponent";

BoundingBoxComponent::BoundingBoxComponent()
{
	SetExtends(1.0f);
}

void BoundingBoxComponent::Init()
{

}

void BoundingBoxComponent::Draw(GLib::Graphics* pGraphics)
{
	auto transformComponent = MakeStrongPtr(mOwner->GetComponent<TransformComponent>(TransformComponent::g_Name));
	XMFLOAT3 pos = transformComponent->GetPosition();

	XNA::AxisAlignedBox box;
	box.Center = pos;
	box.Extents = XMFLOAT3(mExtends, mExtends, mExtends);
	pGraphics->DrawBoundingBox(&box, transformComponent->GetWorldMatrix(), GLib::Material(XMFLOAT4(1, 1, 0, 1)));
}

void BoundingBoxComponent::SetExtends(float extends)
{
	mExtends = extends;
}

float BoundingBoxComponent::GetExtends()
{
	return mExtends;
}