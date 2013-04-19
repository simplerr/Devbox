#include "GlibStd.h"
#include "TransformComponent.h"
	
const char* TransformComponent::g_Name = "TransformComponent";

TransformComponent::TransformComponent()
{
	mPosition = XMFLOAT3(0.0f, 40.0f, 0.0f);
	mRotation = XMFLOAT3(1.0f, 0.0f, 0.4f);
	mScale = XMFLOAT3(1.0f, 1.0f, 1.0f);
}

void TransformComponent::Init()
{

}

void TransformComponent::PostInit()
{

}

void TransformComponent::Update(float dt)
{

}

void TransformComponent::SetPosition(XMFLOAT3 position)
{
	mPosition = position;
}

void TransformComponent::SetRotation(XMFLOAT3 rotation)
{
	mRotation = rotation;
}

void TransformComponent::SetScale(XMFLOAT3 scale)
{
	mScale = scale;
}

XMFLOAT3 TransformComponent::GetPosition()
{
	return mPosition;
}

XMFLOAT3 TransformComponent::GetRotation()
{
	return mRotation;
}

XMFLOAT3 TransformComponent::GetScale()
{
	return mScale;
}

XMMATRIX TransformComponent::GetWorldMatrix()
{
	XMMATRIX T, R, S, W;
	T = XMMatrixTranslation(mPosition.x, mPosition.y, mPosition.z);
	R = XMMatrixRotationRollPitchYaw(mRotation.x, mRotation.y, mRotation.z);
	S = XMMatrixScaling(mScale.x, mScale.y, mScale.z);

	return S*R*T;
}