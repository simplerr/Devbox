#pragma once

#include "Component.h"
#include "Light.h"

namespace GLib {
	class SkinnedModel;
}

const int INFINITE_ANIMATION = -1;

class AnimatedModelComponent : public Component
{
public:
	AnimatedModelComponent();

	void Init(LuaPlus::LuaObject initData);
	void Update(float dt);
	void Draw(GLib::Graphics* pGraphics);

	void SetAnimation(int index, float duration = INFINITE_ANIMATION);
	void AdjustAnimationSpeedBy(float percent);
	void SetAlpha(float alpha);

	GLib::Material GetMaterial();

	const char* GetName() { return g_Name; }
	static const char* g_Name;
private:
	GLib::SkinnedModel* mModel;
	int		mCurrentAnimIndex;
	int		mPreviousAnimation;
	float	mAnimationDuration;
	float   mElapsedTime;
};