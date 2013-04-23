#include "AnimatedModelComponent.h"
#include "SkinnedModel.h"
#include "Graphics.h"
#include "ModelImporter.h"
#include "GlibStd.h"
#include "Actor.h"
#include "TransformComponent.h"

const char* AnimatedModelComponent::g_Name = "AnimatedModelComponent";

AnimatedModelComponent::AnimatedModelComponent()
{
	mModel = nullptr;

	mCurrentAnimIndex = 0;
	mPreviousAnimation = 0;
	mElapsedTime = 0.0f;
	mAnimationDuration = INFINITE_ANIMATION;

	SetAnimation(0);
}

void AnimatedModelComponent::Init(LuaPlus::LuaObject initData)
{
	if(!initData["filename"].IsNil())
	{
		mModel = GLib::GlobalApp::GetGraphics()->GetModelImporter()->LoadSkinnedModel(initData["filename"].ToString());
	}
	if(!initData["alpha"].IsNil())
	{
		SetAlpha(initData["alpha"].ToNumber());
	}
}

void AnimatedModelComponent::Update(float dt)
{
	if(mAnimationDuration != INFINITE_ANIMATION) { // [NOTE] INFINITE_ANIMATION is just a random number!
		mAnimationDuration -= dt;

		if(mAnimationDuration <= 0) {
			SetAnimation(mPreviousAnimation);
			mAnimationDuration = INFINITE_ANIMATION;
		}
	}

	mElapsedTime += dt;
	mModel->Update(dt);
}

void AnimatedModelComponent::Draw(GLib::Graphics* pGraphics)
{
	auto transfrom = MakeStrongPtr(mOwner->GetComponent<TransformComponent>(TransformComponent::g_Name));
	mModel->SetAnimation(mCurrentAnimIndex);
	mModel->SetElapsedTime(mElapsedTime);
	mModel->Draw(pGraphics, transfrom->GetWorldMatrix());
}

void AnimatedModelComponent::SetAnimation(int index, float duration)
{
	mElapsedTime = 0.0f;
	mPreviousAnimation = mCurrentAnimIndex;
	mCurrentAnimIndex = index;
	mAnimationDuration = duration;
}

void AnimatedModelComponent::AdjustAnimationSpeedBy(float percent)
{
	mModel->AdjustAnimationSpeedBy(percent);
}

void AnimatedModelComponent::SetAlpha(float alpha)
{
	GLib::Material material = mModel->GetMaterial();
	material.diffuse.w = alpha;
	mModel->SetMaterial(material);
}
