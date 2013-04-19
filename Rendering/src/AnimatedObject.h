#pragma once
#include "Object3D.h"

namespace GLib {
	// Forward declarations.
	class SkinnedModel;
	class World;

	//!
	//	Represents an animated object. 
	//!
	class GLIB_API AnimatedObject : public Object3D
	{
	public:
		AnimatedObject(ModelImporter* importer, string filename);
		virtual ~AnimatedObject();

		virtual void Init();
		virtual void Update(float dt);
		virtual void Draw(Graphics* pGraphics);

		bool RayIntersect(XMVECTOR origin, XMVECTOR direction, float& pDist);
		void SetAnimation(int index);
		void SetAnimation(int index, float duration);
		void AdjustAnimationSpeedBy(float percent);
		virtual AxisAlignedBox GetBoundingBox();
		SkinnedModel* GetModel();
		string GetFilename();
		int GetCurrentAnimation();
		float GetAnimationSpeed();

	private:
		SkinnedModel* mSkinnedModel;
		int		mCurrentAnimIndex;
		float	mElapsedTime;
		int		mPreviousAnimation;
		float	mAnimationDuration;
	};
}