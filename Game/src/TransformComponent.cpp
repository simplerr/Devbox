#include "GlibStd.h"
#include "TransformComponent.h"
	
const char* TransformComponent::g_Name = "TransformComponent";

TransformComponent::TransformComponent()
{
	mPosition = XMFLOAT3(0.0f, 40.0f, 0.0f);
	mRotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
	mScale = XMFLOAT3(1.0f, 1.0f, 1.0f);
}

void TransformComponent::Init(LuaPlus::LuaObject initData)
{
	// Position.
	if(!initData["pos_x"].IsNil())
		mPosition.x = initData["pos_x"].ToNumber();
	if(!initData["pos_y"].IsNil())
		mPosition.y  = initData["pos_y"].ToNumber();
	if(!initData["pos_z"].IsNil())
		mPosition.z  = initData["pos_z"].ToNumber();

	// Rotation.
	if(!initData["rot_x"].IsNil())
		mRotation.x = initData["rot_x"].ToNumber();
	if(!initData["rot_y"].IsNil())
		mRotation.y = initData["rot_y"].ToNumber();
	if(!initData["rot_z"].IsNil())
		mRotation.z = initData["rot_z"].ToNumber();

	// Scale.
	if(!initData["scale"].IsNil())
	{
		float scale = initData["scale"].ToNumber();
		mScale = XMFLOAT3(scale, scale, scale);
	}
	if(!initData["scale_x"].IsNil())
		mScale.x = initData["scale_x"].ToNumber();
	if(!initData["scale_y"].IsNil())
		mScale.y = initData["scale_y"].ToNumber();
	if(!initData["scale_z"].IsNil())
		mScale.z = initData["scale_z"].ToNumber();
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