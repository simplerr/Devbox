#include "StaticMesh.h"
#include "Graphics.h"
#include "Effects.h"
#include "Primitive.h"

//! Graphics Library namespace.
namespace GLib
{

//! Constructor.
StaticMesh::StaticMesh()
{
	mTexture = nullptr;
	mNormalMap = nullptr;
	mPrimitive = nullptr;
}
	
//! Cleanup.
StaticMesh::~StaticMesh()
{

}

//! Draws the mesh.
void StaticMesh::Draw(Graphics* pGraphics)
{
	// Set the material properties for this mesh.
	Effects::BasicFX->SetTexture(mTexture);
	Effects::BasicFX->SetNormalMap(mNormalMap);

	if(!pGraphics->IsRenderingShadows())
		Effects::BasicFX->Apply(GlobalApp::GetD3DContext(), mNormalMap == 0 ? STANDARD_TECH : NMAP_TECH);	
	else
		Effects::BasicFX->Apply(GlobalApp::GetD3DContext(), SMAP_TECH);

	// Draw the mesh primitive.
	mPrimitive->Draw<Vertex>(pGraphics->GetContext());
}

bool StaticMesh::RayIntersect(XMVECTOR origin, XMVECTOR direction, float& pDist)
{
	bool intersect = false;
	for(UINT i = 0; i < mIndices.size()/3; ++i)
	{
		// Indices for this triangle.
		UINT i0 = mIndices[i*3+0];
		UINT i1 = mIndices[i*3+1];
		UINT i2 = mIndices[i*3+2];

		// Vertices for this triangle.
		XMVECTOR v0 = XMLoadFloat3(&mVertices[i0].Pos);
		XMVECTOR v1 = XMLoadFloat3(&mVertices[i1].Pos);
		XMVECTOR v2 = XMLoadFloat3(&mVertices[i2].Pos);

		float dist = 0.0f;
		direction = XMVector3Normalize(direction);
		if(XNA::IntersectRayTriangle(origin, direction, v0, v1, v2, &dist))
		{
			if(dist < pDist)
				pDist = dist;
			intersect = true;
		}
	}

	return intersect;
}

//! Sets the primitive.
void StaticMesh::SetPrimitive(Primitive* primitive)
{
	mPrimitive = primitive;
}

//! Sets the texture by calling Graphics::LoadTexture(...).
void StaticMesh::LoadTexture(string filename)
{
	mTexture = GlobalApp::GetGraphics()->LoadTexture(filename);
}

//! Sets the material.
void StaticMesh::SetMaterial(Material material)
{
	mMaterial = material;
}
	
//! Sets the texture.
void StaticMesh::SetTexture(Texture2D* texture)
{
	mTexture = texture;
}

void StaticMesh::SetNormalMap(Texture2D* nmap)
{
	mNormalMap = nmap;
}

void StaticMesh::SetVertices(vector<Vertex> vertices)
{
	mVertices = vertices;
}

void StaticMesh::SetIndices(vector<UINT> indices)
{
	mIndices = indices;
}

//! Returns the primitive.
Primitive* StaticMesh::GetPrimitive()
{
	return mPrimitive;
}

//! Returns the material.
Material StaticMesh::GetMaterial()
{
	return mMaterial;
}

}	// End of Graphics Library namespace.