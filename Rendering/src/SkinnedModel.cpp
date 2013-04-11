#include "SkinnedModel.h"
#include "Effects.h"
#include "Camera.h"
#include "Graphics.h"
#include "ShadowMap.h"
#include "SkinnedMesh.h"
#include "cAnimationController.h"
#include "Input.h"

//! Graphics Library namespace.
namespace GLib
{

//! Constructor.
SkinnedModel::SkinnedModel()
{
	SetFilename("#NOVALUE");
	mAnimator = nullptr;
	mElapsedTime = 0.0f;

	AddAnimation(0, 37, 662, 20);
	AddAnimation(38, 83, 662, 20);
	AddAnimation(84, 264, 662, 20);
	AddAnimation(265, 445, 662, 20);
	AddAnimation(446, 491, 662, 20);
	AddAnimation(492, 537, 662, 20);
	AddAnimation(538, 553, 662, 20);
	AddAnimation(554, 662, 662, 20);
}
	
//! Cleaunp.
SkinnedModel::~SkinnedModel()
{
	for(int i = 0; i < mMeshList.size(); i++) 
		delete mMeshList[i];

	mAnimator->Release();
	delete mAnimator;
}

void SkinnedModel::Update(float dt)
{
	mElapsedTime += dt;

	//// Update the elapsed time based on the current animation index.
	//int currentAnimation = mAnimator->GetAnimationIndex();
	//float animationLength = mAnimations[currentAnimation].endtime - mAnimations[currentAnimation].startTime;
	////mElapsedTime = mAnimations[currentAnimation].startTime + mElapsedTime;

	//if(mElapsedTime > animationLength)
	//	mElapsedTime = 0;
}

//! Draws all the skinned meshes. Sets the bone transforms to use in the shader.
void SkinnedModel::Draw(Graphics* pGraphics, CXMMATRIX world)
{
	XMMATRIX view, proj;
	
	if(!pGraphics->IsRenderingShadows()) {
		view = XMLoadFloat4x4(&pGraphics->GetCamera()->GetViewMatrix());
		proj = XMLoadFloat4x4(&pGraphics->GetCamera()->GetProjectionMatrix());
	}
	else {
		view = XMLoadFloat4x4(&pGraphics->GetShadowMap()->GetLightView());
		proj = XMLoadFloat4x4(&pGraphics->GetShadowMap()->GetLightProj());
	}
	
	Effects::BasicFX->SetWorld(world);
	Effects::BasicFX->SetWorldViewProj(world*view*proj);
	Effects::BasicFX->SetWorldInvTranspose(InverseTranspose(world));
	Effects::BasicFX->SetEyePosition(pGraphics->GetCamera()->GetPosition());
	Effects::BasicFX->SetLights(pGraphics->GetLightList());
	Effects::BasicFX->SetShadowMap(pGraphics->GetShadowMap()->GetSRV());
	Effects::BasicFX->SetShadowTransform(world * XMLoadFloat4x4(&pGraphics->GetShadowMap()->GetShadowTransform()));
	Effects::BasicFX->SetFogColor(pGraphics->GetFogColor());
	Effects::BasicFX->SetFogStart(1000.0f);
	Effects::BasicFX->SetFogRange(50.0f);
	Effects::BasicFX->SetUseAnimation(true);

	// Bone transforms.
	vector<XMFLOAT4X4> finalTransforms = mAnimator->GetTransforms(mElapsedTime);	//mElapsedTime

	//OutputDebugString(string(to_string(mElapsedTime + mAnimations[mAnimator->GetAnimationIndex()].startTime)+"\n").c_str());

	Effects::BasicFX->SetBoneTransforms(&finalTransforms[0], finalTransforms.size());

	// Loop through and draw each mesh.
	for(int i = 0; i < mMeshList.size(); i++)
	{
		mMeshList[i]->Draw(pGraphics);
	}
}

vector<XMFLOAT4X4> SkinnedModel::GetFinalTransforms()
{
	return mAnimator->GetTransforms(mElapsedTime);
}

//! Saves the model to a file.[TODO]
void SkinnedModel::Save(string filename)
{
	ofstream fout(filename, ios::binary);

	fout << "#Meshes " << mMeshList.size() << "\r\n";

	// Save the meshes.
	for(int i = 0; i < mMeshList.size(); i++)
		mMeshList[i]->Save(fout);

	// Save the animations.
	mAnimator->Save(fout);

	fout.close();
}
	
//! Loads the  model from a file.[TODO]
void SkinnedModel::Load(string filename)
{
	ifstream fin(filename, ios::binary | ios::in);

	string ignore;
	int numMeshes;
	fin >> ignore >> numMeshes;

	for(int i =  0; i < numMeshes; i++)
	{
		SkinnedMesh* mesh = new SkinnedMesh();
		mesh->Load(fin);
		AddMesh(mesh);
	}

	// The space after the last index.
	fin >> ignore;	

	// Make sure to start loading animation data from the right place.
	// [NOTE] After some investigation it seems like it should be at the first '\f' character.
	char x = fin.peek(); 
	while(x != '\f') {
		fin >> ignore;	
		x = fin.peek(); 
	}

	mAnimator = new SceneAnimator();
	mAnimator->Load(fin);

	fin.close();
}

//! Sets the animation controller.
void SkinnedModel::SetAnimator(SceneAnimator* animator)
{
	mAnimator = animator;
}

//! Sets the filename.
void SkinnedModel::SetFilename(string filename)
{
	mFilename = filename;
}

//! Sets which animation to use by name.
void SkinnedModel::SetAnimation(string animation)
{
	mAnimator->SetAnimation(animation);
}
	
//! Sets which animation to use by index.
void SkinnedModel::SetAnimation(int index)
{
	mAnimator->SetAnimIndex(index);
}

//! Adds a mesh to the list.
void SkinnedModel::AddMesh(SkinnedMesh* mesh)
{
	mMeshList.push_back(mesh);
}

//! Returns the mesh list.
SkinnedMeshList* SkinnedModel::GetMeshList()
{
	return &mMeshList;
}

void SkinnedModel::SetElapsedTime(float elapsedTime)
{
	mElapsedTime = elapsedTime;
}

void SkinnedModel::CalculateAABB()
{
	XMFLOAT3 min = XMFLOAT3(numeric_limits<float>::infinity(), numeric_limits<float>::infinity(), numeric_limits<float>::infinity());
	XMFLOAT3 max = XMFLOAT3(-numeric_limits<float>::infinity(), -numeric_limits<float>::infinity(), -numeric_limits<float>::infinity());

	for(int i = 0; i < 1; i++)
	{
		vector<XMFLOAT4X4> transforms = mAnimator->GetTransforms(1.0f);
		for(int j = 0; j < mMeshList.size(); j++) 
		{
			XNA::AxisAlignedBox aabb = mMeshList[j]->CalculateAABB(transforms);
			XMFLOAT3 meshMin = aabb.Center - aabb.Extents;
			XMFLOAT3 meshMax = aabb.Center + aabb.Extents;

			XMStoreFloat3(&min, XMVectorMin(XMLoadFloat3(&meshMin), XMLoadFloat3(&min)));
			XMStoreFloat3(&max, XMVectorMax(XMLoadFloat3(&meshMax), XMLoadFloat3(&max)));
		}
	}

	mAABB.Center = (min + max) * 0.5f;
	mAABB.Extents = (max - min) * 0.5f;
}

XNA::AxisAlignedBox SkinnedModel::GetBoundingBox()
{
	return mAABB;
}

string SkinnedModel::GetFilename()
{
	return mFilename;
}

void SkinnedModel::SetMeshMaterial(int meshId, Material material)
{
	if(meshId < mMeshList.size() && meshId >= 0)
		mMeshList[meshId]->SetMaterial(material);
}

int SkinnedModel::GetCurrentAnimation()
{
	return mAnimator->GetAnimationIndex();
}

void SkinnedModel::AddAnimation(int startFrame, int endFrame, float totalFrames, float length)
{
	mAnimations.push_back(Animation(startFrame, endFrame, totalFrames, length));
}

void SkinnedModel::AdjustAnimationSpeedBy(float percent)
{
	mAnimator->AdjustAnimationSpeedBy(percent);
}

float SkinnedModel::GetAnimationSpeed()
{
	return mAnimator->GetAnimationSpeed();
}

}	// End of Graphics Library namespace.