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

	struct VoxelVertex
	{
		VoxelVertex() {}
		VoxelVertex(float x, float y, float z, float nx, float ny, float nz)
			: Position(XMFLOAT3(x, y, z)), Normal(XMFLOAT3(nx, ny, nz)) {}
		VoxelVertex(XMFLOAT3 pos, XMFLOAT3 normal)
			: Position(pos), Normal(normal) {}

		XMFLOAT3 Position;
		XMFLOAT3 Normal;
	};

	struct BoundingBoxVertex
	{
		BoundingBoxVertex() {}
		BoundingBoxVertex(float x, float y, float z)
			: Position(XMFLOAT3(x, y, z)) {}
		BoundingBoxVertex(XMFLOAT3 pos)
			: Position(pos) {}

		XMFLOAT3 Position;
	};
}