#include "StaticModelComponent.h"
#include "StaticModel.h"
#include "Graphics.h"
#include "ModelImporter.h"
#include "GlibStd.h"
#include "Actor.h"
#include "TransformComponent.h"

const char* StaticModelComponent::g_Name = "StaticModelComponent";

StaticModelComponent::StaticModelComponent()
{
	mModel = nullptr;
}

void StaticModelComponent::Init(LuaPlus::LuaObject initData)
{
	if(!initData["filename"].IsNil())
	{
		mModel = GLib::GlobalApp::GetGraphics()->GetModelImporter()->LoadStaticModel(initData["filename"].ToString());
	}
	if(!initData["alpha"].IsNil())
	{
		SetAlpha(initData["alpha"].ToNumber());
	}
}

void StaticModelComponent::Draw(GLib::Graphics* pGraphics)
{
	auto transfrom = MakeStrongPtr(mOwner->GetComponent<TransformComponent>(TransformComponent::g_Name));
	mModel->Draw(pGraphics, transfrom->GetWorldMatrix());
}

void StaticModelComponent::SetAlpha(float alpha)
{
	GLib::Material material = mModel->GetMaterial();
	material.diffuse.w = alpha;
	mModel->SetMaterial(material);
}

GLib::Material StaticModelComponent::GetMaterial()
{
	return mModel->GetMaterial();
}