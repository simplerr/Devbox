#pragma once

#include <vector>
#include "Utility.h"

struct Animation
{
	Animation(float startFrame, float endFrame, float totalFrames, float length) {
		startTime = (startFrame / totalFrames) * length;
		endtime = (endFrame / totalFrames) * length;
	}

	float startTime;
	float endtime;
};

//! Graphics Library namespace.
namespace GLib
{
	// Forward declarations.
	class Graphics;
	class SkinnedMesh;
	class SceneAnimator;
	struct Material;

	typedef vector<SkinnedMesh*> SkinnedMeshList;

	//!
	//	Contains a list of skinned meshes representing an animated model,
	//	and a SceneAnimator that calculates the bone transforms.
	//!
	class SkinnedModel
	{
	public:
		SkinnedModel();
		~SkinnedModel();

		void Update(float dt);
		void Draw(Graphics* pGraphics, CXMMATRIX world);
		void Save(string filename);
		void Load(string filename);
		void AddMesh(SkinnedMesh* mesh);
		void SetAnimator(SceneAnimator* animator);

		void SetAnimation(string animation);
		void SetFilename(string filename);
		void SetAnimation(int index);
		void SetElapsedTime(float elapsedTime);
		void SetMeshMaterial(int meshId, Material material);
		void AdjustAnimationSpeedBy(float percent);

		SkinnedMeshList* GetMeshList();
		vector<XMFLOAT4X4> GetFinalTransforms();
		XNA::AxisAlignedBox GetBoundingBox();
		string GetFilename();
		void CalculateAABB();
		int GetCurrentAnimation();
		float GetAnimationSpeed();

		void AddAnimation(int startFrame, int endFrame, float totalFrames, float length);
	private:
		SkinnedMeshList	mMeshList;
		SceneAnimator*	mAnimator;

		float			mElapsedTime;
		string			mFilename;
		XNA::AxisAlignedBox mAABB;

		vector<Animation> mAnimations;
	};
}