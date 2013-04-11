#pragma once

#include <map>
#include "d3dUtil.h"
#include "Vertex.h"
#include "Light.h"

//! Graphics Library namespace.
namespace GLib
{
	// Forward declarations.
	class Graphics;
	struct Texture2D;
	class Primitive;

	//!
	//	A part of a skinned model that's rendered with the same texture and material.
	//	Contains a Primitive with the vertices and indices.
	//!
	class SkinnedMesh
	{
	public:
		SkinnedMesh();
		~SkinnedMesh();

		void Draw(Graphics* pGraphics);

		void SetPrimitive(Primitive* primitive);
		void SetVertices(vector<SkinnedVertex> vertices);
		void SetIndices(vector<UINT> indices);
		void SetMaterial(Material material);
		void SetTexture(Texture2D* texture);
		void SetNormalMap(Texture2D* nmap);
		void LoadTexture(string filename);

		void Save(ofstream& fout);
		void Load(ifstream& fin);

		Primitive*		GetPrimitive();
		Material		GetMaterial();
		XNA::AxisAlignedBox CalculateAABB(vector<XMFLOAT4X4>& finalTransforms);
	private:
		Primitive*		mPrimitive;
		Texture2D*		mTexture;
		Texture2D*		mNormalMap;
		Material		mMaterial;
		vector<SkinnedVertex> mVertices;
		vector<UINT>		  mIndices;
	};
}