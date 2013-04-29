#include "Chunk.h"
#include <windows.h>
#include "Vertex.h"
#include "GlibStd.h"
#include "Primitive.h"
#include "Effects.h"
#include "VoxelEffect.h"
#include "Graphics.h"
#include "Camera.h"
#include "xnacollision.h"
#include "Utility.h"
#include <xnamath.h>

using namespace GLib;

Chunk::Chunk(float x, float y, float z)
{
	// Create the blocks.
	mBlocks = new Block**[CHUNK_SIZE];
	for(int i = 0; i < CHUNK_SIZE; i++)
	{
		mBlocks[i] = new Block*[CHUNK_SIZE];

		for(int j = 0; j < CHUNK_SIZE; j++)
		{
			mBlocks[i][j] = new Block[CHUNK_SIZE];
		}
	}

	mPosition = XMFLOAT3(x, y, z);

	mBlockCount = 0;

	mPrimitive = nullptr;

	mColor = Colors::LightSteelBlue;

	CreateMesh();
	mPrimitive = new GLib::Primitive;
	BuildMeshPrimitive();
}

Chunk::~Chunk()
{
	// Delete the blocks
	for (int i = 0; i < CHUNK_SIZE; ++i)
	{
		for (int j = 0; j < CHUNK_SIZE; ++j)
		{
			delete [] mBlocks[i][j];
		}

		delete [] mBlocks[i];
	}
	delete [] mBlocks;
}

// Loops over all blocks and adds them to the vertex buffer.
void Chunk::CreateMesh()
{
	for(int x = 0; x < CHUNK_SIZE; x++)
	{
		for(int y = 0; y < CHUNK_SIZE; y++)
		{
			for(int z = 0; z < CHUNK_SIZE; z++)
			{
				if(mBlocks[x][y][z].IsActive())
				{
					float X = x - CHUNK_SIZE/2;
					float Y = y - CHUNK_SIZE/2;
					float Z = z - CHUNK_SIZE/2;
					//if(rand() % 15 == 0)
						AddCube(mPosition.x + x*VOXEL_SIZE + VOXEL_SIZE/2 , mPosition.y + y*VOXEL_SIZE + VOXEL_SIZE/2, mPosition.z + z*VOXEL_SIZE + VOXEL_SIZE/2);
				}
				else
				{
					continue;
				}
			}
		}
	}
}


void Chunk::BuildMeshPrimitive()
{
	// Only rebuild mBlockCount blocks.
	// They are located at [0] -> [(mBlockCount-1) * 24 or 36] in the vertex and index array.
	mPrimitive->SetVertices(GLib::GlobalApp::GetD3DDevice(), mVertices, 24*mBlockCount);
	mPrimitive->SetIndices(GLib::GlobalApp::GetD3DDevice(), mIndices, 36*mBlockCount);
}

void Chunk::Rebuild()
{
	// Technically no blocks gets removed from the arrays,
	// they are just overwritten. If the new block count is 
	// less then the last one, the old blocks are actually still stored
	// in the arrays, but the buffers don't take them into account due to
	// ByteWidth = sizeof(VoxelVertex) * mBlockCount * 24 and
	// ByteWidth = sizeof(UINT) * mBlockCount * 36

	// Restore the block count.
	mBlockCount = 0;
	CreateMesh();
	BuildMeshPrimitive();
}

void Chunk::Render(GLib::Graphics* pGraphics)
{
	// Set the input layout and the primitive topology.
	GlobalApp::GetD3DContext()->IASetInputLayout(Effects::VoxelFX->GetInputLayout());
	GlobalApp::GetD3DContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	XMMATRIX view = XMLoadFloat4x4(&GLib::GlobalApp::GetGraphics()->GetCamera()->GetViewMatrix());
	XMMATRIX proj = XMLoadFloat4x4(&GLib::GlobalApp::GetGraphics()->GetCamera()->GetProjectionMatrix());

	GLib::Effects::VoxelFX->SetColor(mColor);
	GLib::Effects::VoxelFX->SetWorld(XMMatrixIdentity());
	GLib::Effects::VoxelFX->SetWorldViewProj(XMMatrixIdentity() * view * proj);
	Effects::BasicFX->SetWorldInvTranspose(InverseTranspose(XMMatrixIdentity()));
	GLib::Effects::VoxelFX->Apply(GLib::GlobalApp::GetD3DContext());

	mPrimitive->Draw<VoxelVertex>(GLib::GlobalApp::GetD3DContext());

	float size = CHUNK_SIZE * VOXEL_SIZE;
	float localCenter = size / 2;
	
	pGraphics->DrawBoundingBox(XMFLOAT3(mPosition.x + localCenter, mPosition.y + localCenter, mPosition.z + localCenter), size, size, size, Colors::Green, true, 1.0f);

	pGraphics->DrawBoundingBox(XMFLOAT3(mPosition.x, mPosition.y, mPosition.z), 3, 3, 3, Colors::Black, false, 1.0f);
}

bool Chunk::RayIntersect(XMVECTOR origin, XMVECTOR direction, float& pDist)
{
	// Broadphase
	float dist;
	XNA::AxisAlignedBox box;
	float size = CHUNK_SIZE * VOXEL_SIZE;
	float localCenter = size / 2;
	box.Center = mPosition + XMFLOAT3(localCenter, localCenter, localCenter);
	box.Extents = XMFLOAT3(size/2, size/2, size/2);
	if(!GLibIntersectRayAxisAlignedBox(origin, direction, &box, &dist))
		return false;

	bool intersect = false;
	for(UINT i = 0; i < 36*mBlockCount/3; ++i)
	{
		// Indices for this triangle.
		UINT i0 = mIndices[i*3+0];
		UINT i1 = mIndices[i*3+1];
		UINT i2 = mIndices[i*3+2];

		// Vertices for this triangle.
		XMVECTOR v0 = XMLoadFloat3(&mVertices[i0].Position);
		XMVECTOR v1 = XMLoadFloat3(&mVertices[i1].Position);
		XMVECTOR v2 = XMLoadFloat3(&mVertices[i2].Position);

		float dist = 0.0f;
		direction = XMVector3Normalize(direction);
		if(GLibIntersectRayTriangle(origin, direction, v0, v1, v2, &dist))
		{
			if(dist < pDist)
				pDist = dist;
			intersect = true;
		}
	}

	return intersect;
}

XMFLOAT3 Chunk::GetPosition()
{
	return mPosition;
}

void Chunk::SetColor(XMFLOAT4 color)
{
	mColor = color;
}

// Adds a cube to the vertex buffer.
void Chunk::AddCube(int x, int y, int z)
{
	// Create the vertices.
	float w2, h2, d2;
	w2 = h2 = d2 = VOXEL_SIZE/2;

	int n = mBlockCount;

	// [HACK] Avoid writing outside the vertices array, don't know why
	// it only happens after a while.
	if(mBlockCount == CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE)
		return;

	// Fill in the front face vertex data.
	mVertices[n*24 + 0] = VoxelVertex(x -w2, y -h2, z -d2, 0.0f, 0.0f, -1.0f);
	mVertices[n*24 + 1] = VoxelVertex(x -w2, y +h2, z -d2, 0.0f, 0.0f, -1.0f);
	mVertices[n*24 + 2] = VoxelVertex(x +w2, y +h2, z -d2, 0.0f, 0.0f, -1.0f);
	mVertices[n*24 + 3] = VoxelVertex(x +w2, y -h2, z -d2, 0.0f, 0.0f, -1.0f);

	// Fill in the back face vertex data.
	mVertices[n*24 + 4] = VoxelVertex(x -w2, y -h2, z +d2, 0.0f, 0.0f, 1.0f);
	mVertices[n*24 + 5] = VoxelVertex(x +w2, y -h2, z +d2, 0.0f, 0.0f, 1.0f);
	mVertices[n*24 + 6] = VoxelVertex(x +w2, y +h2, z +d2, 0.0f, 0.0f, 1.0f);
	mVertices[n*24 + 7] = VoxelVertex(x -w2, y +h2, z +d2, 0.0f, 0.0f, 1.0f);

	// Fill in the top face vertex data.
	mVertices[n*24 + 8]  = VoxelVertex(x -w2, y +h2, z -d2, 0.0f, 1.0f, 0.0f);
	mVertices[n*24 + 9]  = VoxelVertex(x -w2, y +h2, z +d2, 0.0f, 1.0f, 0.0f);
	mVertices[n*24 + 10] = VoxelVertex(x +w2, y +h2, z +d2, 0.0f, 1.0f, 0.0f);
	mVertices[n*24 + 11] = VoxelVertex(x +w2, y +h2, z -d2, 0.0f, 1.0f, 0.0f);

	// Fill in the bottom face vertex data.
	mVertices[n*24 + 12] = VoxelVertex(x -w2, y -h2, z -d2, 0.0f, -1.0f, 0.0f);
	mVertices[n*24 + 13] = VoxelVertex(x +w2, y -h2, z -d2, 0.0f, -1.0f, 0.0f);
	mVertices[n*24 + 14] = VoxelVertex(x +w2, y -h2, z +d2, 0.0f, -1.0f, 0.0f);
	mVertices[n*24 + 15] = VoxelVertex(x -w2, y -h2, z +d2, 0.0f, -1.0f, 0.0f);

	// Fill in the left face vertex data.
	mVertices[n*24 + 16] = VoxelVertex(x -w2, y -h2, z +d2, -1.0f, 0.0f, 0.0f);
	mVertices[n*24 + 17] = VoxelVertex(x -w2, y +h2, z +d2, -1.0f, 0.0f, 0.0f);
	mVertices[n*24 + 18] = VoxelVertex(x -w2, y +h2, z -d2, -1.0f, 0.0f, 0.0f);
	mVertices[n*24 + 19] = VoxelVertex(x -w2, y -h2, z -d2, -1.0f, 0.0f, 0.0f);

	// Fill in the right face vertex data.
	mVertices[n*24 + 20] = VoxelVertex(x +w2, y -h2, z -d2, 1.0f, 0.0f, 0.0f);
	mVertices[n*24 + 21] = VoxelVertex(x +w2, y +h2, z -d2, 1.0f, 0.0f, 0.0f);
	mVertices[n*24 + 22] = VoxelVertex(x +w2, y +h2, z +d2, 1.0f, 0.0f, 0.0f);
	mVertices[n*24 + 23] = VoxelVertex(x +w2, y -h2, z +d2, 1.0f, 0.0f, 0.0f);

	// Fill in the front face index data
	mIndices[n*36 + 0] = n*24 + 0; mIndices[n*36 + 1] = n*24 + 1; mIndices[n*36 + 2] = n*24 + 2;
	mIndices[n*36 + 3] = n*24 + 0; mIndices[n*36 + 4] = n*24 + 2; mIndices[n*36 + 5] = n*24 + 3;

	// Fill in the back face index data
	mIndices[n*36 + 6] = n*24 + 4; mIndices[n*36 + 7]  = n*24 + 5; mIndices[n*36 + 8]  = n*24 + 6;
	mIndices[n*36 + 9] = n*24 + 4; mIndices[n*36 + 10] = n*24 + 6; mIndices[n*36 + 11] = n*24 + 7;

	// Fill in the top face index data
	mIndices[n*36 + 12] = n*24 + 8; mIndices[n*36 + 13] = n*24 + 9; mIndices[n*36 + 14] = n*24 + 10;
	mIndices[n*36 + 15] = n*24 + 8; mIndices[n*36 + 16] = n*24 + 10; mIndices[n*36 + 17] = n*24 + 11;

	// Fill in the bottom face index data
	mIndices[n*36 + 18] = n*24 + 12; mIndices[n*36 + 19] = n*24 + 13; mIndices[n*36 + 20] = n*24 + 14;
	mIndices[n*36 + 21] = n*24 + 12; mIndices[n*36 + 22] = n*24 + 14; mIndices[n*36 + 23] = n*24 + 15;

	// Fill in the left face index data
	mIndices[n*36 + 24] = n*24 + 16; mIndices[n*36 + 25] = n*24 + 17; mIndices[n*36 + 26] = n*24 + 18;
	mIndices[n*36 + 27] = n*24 + 16; mIndices[n*36 + 28] = n*24 + 18; mIndices[n*36 + 29] = n*24 + 19;

	// Fill in the right face index data
	mIndices[n*36 + 30] = n*24 + 20; mIndices[n*36 + 31] = n*24 + 21; mIndices[n*36 + 32] = n*24 + 22;
	mIndices[n*36 + 33] = n*24 + 20; mIndices[n*36 + 34] = n*24 + 22; mIndices[n*36 + 35] = n*24 + 23;

	mBlockCount++;
}