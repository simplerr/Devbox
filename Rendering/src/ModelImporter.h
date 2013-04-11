#pragma once
#include <string>
#include <map>
#include <assimp\Importer.hpp>
#include <assimp\mesh.h>
#include <assimp\scene.h>
#include "d3dUtil.h"
using namespace std;

//! Graphics Library namespace.
namespace GLib
{
	// Forward declarations.
	class Primitive;
	class PrimitiveFactory;
	class StaticModel;
	class SkinnedModel;
	class SkinnedModel;
	class SceneAnimator;
	struct BoneInfo;

	//! Vertex weights and indices for bone animations.
	struct Weights
	{
		vector<int>		boneIndices;
		vector<float>	weights;
	};

	//!
	//	Loads models and stores them to support instancing.
	//!
	class ModelImporter
	{
	public:
		ModelImporter(PrimitiveFactory* primitiveFactory);
		~ModelImporter();

		StaticModel*	LoadStaticModel(string filename);
		SkinnedModel*	LoadSkinnedModel(string filename);

	private:
		vector<Weights> CalculateWeights(aiMesh* mesh, SceneAnimator* animator);
		int				FindValidPath(aiString* p_szString);
		bool			TryLongerPath(char* szTemp,aiString* p_szString);
	private:
		map<string, StaticModel*>	mStaticModelMap;
		map<string, SkinnedModel*>	mSkinnedModelMap;
		PrimitiveFactory*	mPrimtiveFactory;
		string				mFilename;
	};
}