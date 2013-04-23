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

	mAlpha = 1.0f;
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
	// [NOTE][TODO]
	// Set the alpha.
	// This has to be in the Component because every StaticModel* is shared among all actors,
	// placed here for the same reason as the animation index issue in SkinnedModel.
	GLib::Material material = mModel->GetMaterial();
	material.diffuse.w = mAlpha;
	mModel->SetMaterial(material);

	auto transfrom = MakeStrongPtr(mOwner->GetComponent<TransformComponent>(TransformComponent::g_Name));
	mModel->Draw(pGraphics, transfrom->GetWorldMatrix());
}

void StaticModelComponent::SetAlpha(float alpha)
{
	mAlpha = alpha;
}

GLib::Material StaticModelComponent::GetMaterial()
{
	return mModel->GetMaterial();
}