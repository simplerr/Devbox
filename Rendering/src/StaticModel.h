#pragma once

#include "d3dUtil.h"

//! Graphics Library namespace.
namespace GLib
{
	// Forward declarations.
	class Graphics;
	class StaticMesh;
	struct Material;

	typedef vector<StaticMesh*> MeshList;

	//! Contains a list of static meshes representing a model.
	class StaticModel
	{
	public:
		StaticModel();
		~StaticModel();

		void Draw(Graphics* pGraphics, CXMMATRIX world);
		void AddMesh(StaticMesh* mesh);
		bool RayIntersect(XMVECTOR origin, XMVECTOR direction, float& pDist);

		void SetMaterial(Material material);
		void SetFilename(string filename);

		MeshList* GetMeshList();
		string GetFilename();
	private:
		MeshList mMeshList;
		string mFilename;
	};
}