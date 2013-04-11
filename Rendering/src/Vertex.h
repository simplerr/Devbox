#pragma once
#include <Windows.h>
#include <xnamath.h>

//! Graphics Library namespace.
namespace GLib
{
	// Forward declarations.
	class BillboardManager;

	//! Basic vertex.
	struct Vertex
	{
		Vertex() {}
		Vertex(XMFLOAT3 pos) : Pos(pos) {}
		Vertex(float px, float py, float pz, float nx, float ny, float nz)
			: Pos(px, py, pz), Normal(nx, ny, nz) {}
		Vertex(float px, float py, float pz, float nx, float ny, float nz, float tx, float ty, float tz, float u, float v) 
			: Pos(px, py, pz), Normal(nx, ny, nz), Tangent(tx, ty, tz, 1.0f), Tex(u, v) {}

		XMFLOAT3 Pos;
		XMFLOAT3 Normal;
		XMFLOAT2 Tex;
		XMFLOAT4 Tangent;
	};

	//! Skinned vertex.
	struct SkinnedVertex
	{
		SkinnedVertex() {}
		SkinnedVertex(float px, float py, float pz, float nx, float ny, float nz)
			: Pos(px, py, pz), Normal(nx, ny, nz) {}
		SkinnedVertex(float px, float py, float pz, float nx, float ny, float nz, float tx, float ty, float tz, float u, float v) 
			: Pos(px, py, pz), Normal(nx, ny, nz), Tangent(tx, ty, tz, 1.0f), Tex(u, v) {
				Weights = XMFLOAT3(0, 0, 0);
				BoneIndices[0] = BoneIndices[1] = BoneIndices[2] = BoneIndices[3] = 0;
		}

		XMFLOAT3 Pos;
		XMFLOAT3 Normal;
		XMFLOAT2 Tex;
		XMFLOAT4 Tangent;
		XMFLOAT3 Weights;
		BYTE BoneIndices[4];
	};

	//! Billboard Vertex.
	struct BillboardVertex
	{
		BillboardVertex() {}
		BillboardVertex(XMFLOAT3 pos, XMFLOAT2 size) 
			: Pos(pos), Size(size) {}	

		void SetPos(XMFLOAT3 pos);
		void SetSize(XMFLOAT2 size);
		void Remove();

		XMFLOAT3 Pos;
		XMFLOAT2 Size;
		int		 Id;
		BillboardManager* Manager;
	};
}