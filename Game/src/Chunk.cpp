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
#include "ChunkManager.h"

using namespace GLib;

noise::module::Perlin Chunk::perlin;

// The < operator for ChunkIndex.
bool operator<(const ChunkCoord a, const ChunkCoord b)
{
	return (a.x < b.x) || (a.x==b.x && a.z < b.z);
}

bool operator==(const ChunkCoord& lhs, const ChunkCoord& rhs)
{
	return lhs.x == rhs.x && lhs.z == rhs.z;
}

Chunk::Chunk(float _x, float _y, float _z)
{
	// Create the blocks.
	mBlocks = new Block**[CHUNK_SIZE];
	for(int x = 0; x < CHUNK_SIZE; x++)
	{
		mBlocks[x] = new Block*[CHUNK_HEIGHT];

		for(int y = 0; y < CHUNK_HEIGHT; y++)
		{
			mBlocks[x][y] = new Block[CHUNK_SIZE];
		}
	}

	mPosition = XMFLOAT3(_x, _y, _z);

	mBlockCount = 0;
	mRebuildFlag = false;
	mPrimitive = nullptr;

	mColor = Colors::LightSteelBlue;
}

Chunk::~Chunk()
{
	// Delete the blocks
	for (int x = 0; x < CHUNK_SIZE; ++x)
	{
		for (int y = 0; y < CHUNK_HEIGHT; ++y)
		{
			delete [] mBlocks[x][y];
		}

		delete [] mBlocks[x];
	}
	delete [] mBlocks;
}

void Chunk::Init()
{
	BuildLandscape();
	//BuildSphere();

	CreateMesh();
	mPrimitive = new GLib::Primitive;
	BuildMeshPrimitive();
}

// Loops over all blocks and adds them to the vertex buffer.
void Chunk::CreateMesh()
{
	for(int x = 0; x < CHUNK_SIZE; x++)
	{
		for(int y = 0; y < CHUNK_HEIGHT; y++)
		{
			for(int z = 0; z < CHUNK_SIZE; z++)
			{
				if(mBlocks[x][y][z].mActive)
				{
					// If all surrounding blocks are active then this block
					// don't need to be rendered.
					bool allNeighborsActive = true;
					if(x > 0)
						allNeighborsActive = mBlocks[x-1][y][z].mActive ? allNeighborsActive : false;
					if(x < CHUNK_SIZE-1)
						allNeighborsActive = mBlocks[x+1][y][z].mActive ? allNeighborsActive : false;

					if(y > 0)
						allNeighborsActive = mBlocks[x][y-1][z].mActive ? allNeighborsActive : false;
					if(y < CHUNK_HEIGHT-1)
						allNeighborsActive = mBlocks[x][y+1][z].mActive ? allNeighborsActive : false;

					if(z > 0)
						allNeighborsActive = mBlocks[x][y][z-1].mActive ? allNeighborsActive : false;
					if(z < CHUNK_SIZE-1)
						allNeighborsActive = mBlocks[x][y][z+1].mActive ? allNeighborsActive : false;

					// Only add cube if not surrounded by active blocks.
					if(!allNeighborsActive)
						AddCube(mPosition.x + x*VOXEL_SIZE + (float)VOXEL_SIZE/2 , mPosition.y + y*VOXEL_SIZE + (float)VOXEL_SIZE/2, mPosition.z + z*VOXEL_SIZE + (float)VOXEL_SIZE/2);
				}
				else
				{
					continue;
				}
			}
		}
	}
}

void Chunk::BuildLandscape()
{
	for(int x = 0; x < CHUNK_SIZE; x++)
	{
		for(int z = 0; z < CHUNK_SIZE; z++)
		{
			// Read noise value here...
			float height = perlin.GetValue((float)(mChunkIndex.x * CHUNK_SIZE + x)/500, 0, (float)(mChunkIndex.z * CHUNK_SIZE + z)/500);

			height = min(CHUNK_HEIGHT-1, height*(CHUNK_HEIGHT-1));
			height = max(1, height);
			for(int y = 0; y < height; y++)
			{
				mBlocks[x][y][z].SetActive(true);
			}
		}
	}
}

void Chunk::BuildSphere()
{
	for(int x = 0; x < CHUNK_SIZE; x++)
	{
		for(int y = 0; y < CHUNK_HEIGHT; y++)
		{
			for(int z = 0; z < CHUNK_SIZE; z++)
			{
				float X = x - CHUNK_SIZE/2;
				float Y = y - CHUNK_HEIGHT/2;
				float Z = z - CHUNK_SIZE/2;

				if(sqrt(X*X + Y*Y + Z*Z) < CHUNK_SIZE/2)
					mBlocks[x][y][z].SetActive(true);
			}
		}
	}
}

void Chunk::BuildMeshPrimitive()
{
	// Only rebuild mBlockCount blocks.
	// They are located at [0] -> [(mBlockCount-1) * 24 or 36] in the vertex and index array.
	mPrimitive->SetVertices(GLib::GlobalApp::GetD3DDevice(), ChunkManager::TempChunkVertices, 24*mBlockCount);
	mPrimitive->SetIndices(GLib::GlobalApp::GetD3DDevice(), ChunkManager::TempChunkIndices, 36*mBlockCount);
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
	BuildSphere();
	CreateMesh();
	BuildMeshPrimitive();

	mRebuildFlag = false;
}

void Chunk::Render(GLib::Graphics* pGraphics)
{
	

	//GLib::Effects::VoxelFX->SetColor(mColor);
	

	mPrimitive->Draw<VoxelVertex>(GLib::GlobalApp::GetD3DContext());
}

BlockIndex Chunk::PositionToBlockId(XMFLOAT3 position)
{
	BlockIndex index = {-1, -1, -1};

	// HACK TEST!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// Add a direction scalar to the position
	// The if statements below only work at the edges!!!
	//position.y -= 0.1f;
	//position.x -= 0.1f;

	XMFLOAT3 localPosition = position - mPosition;

	float size = CHUNK_SIZE * VOXEL_SIZE;

	// Is the position outside the Chunk?
	// It shouldn't be since the ChunkManager::PositionToChunkId() should be used first.
	if(localPosition.x < 0 || localPosition.x > mPosition.x + size || localPosition.y < 0 || localPosition.y > mPosition.y + size || localPosition.z < 0 || localPosition.z > mPosition.z + size)
	{
		GLIB_ERROR("Position outside chunk!");
		return index;
	}

	// Transform to block position.
	index.x = localPosition.x / VOXEL_SIZE;
	index.y = localPosition.y / VOXEL_SIZE;
	index.z = localPosition.z / VOXEL_SIZE;

	return index;
}

bool Chunk::RayIntersectMesh(XMVECTOR origin, XMVECTOR direction, float& pDist)
{
	bool intersect = false;
	//pDist = 9999999;
	//for(UINT i = 0; i < 36*mBlockCount/3; ++i)
	//{
	//	// Indices for this triangle.
	//	UINT i0 = mIndices[i*3+0];
	//	UINT i1 = mIndices[i*3+1];
	//	UINT i2 = mIndices[i*3+2];

	//	// Vertices for this triangle.
	//	XMVECTOR v0 = XMLoadFloat3(&mVertices[i0].Position);
	//	XMVECTOR v1 = XMLoadFloat3(&mVertices[i1].Position);
	//	XMVECTOR v2 = XMLoadFloat3(&mVertices[i2].Position);

	//	float dist = 0.0f;
	//	direction = XMVector3Normalize(direction);
	//	if(GLibIntersectRayTriangle(origin, direction, v0, v1, v2, &dist))
	//	{
	//		if(dist < pDist)
	//			pDist = dist;

	//		intersect = true;
	//	}
	//}

	return intersect;
}

// Broadphase
bool Chunk::RayIntersectBox(XMVECTOR origin, XMVECTOR direction, float& pDist)
{
	XNA::AxisAlignedBox aabb = GetAxisAlignedBox();
	if(GLibIntersectRayAxisAlignedBox(origin, direction, &aabb, &pDist))
		return true;
	else
		return false;
}

XNA::AxisAlignedBox Chunk::GetAxisAlignedBox()
{
	XNA::AxisAlignedBox aabb;
	float size = CHUNK_SIZE * VOXEL_SIZE;
	float localCenter = size / 2;
	aabb.Center = mPosition + XMFLOAT3(localCenter, CHUNK_HEIGHT*VOXEL_SIZE/2, localCenter);
	aabb.Extents = XMFLOAT3(size/2, CHUNK_HEIGHT*VOXEL_SIZE/2, size/2);

	return aabb;
}

XMFLOAT3 Chunk::GetPosition()
{
	return mPosition;
}

bool Chunk::GetRebuildFlag()
{
	return mRebuildFlag;
}

int Chunk::GetNumBlocks()
{
	return mBlockCount;
}

void Chunk::SetColor(XMFLOAT4 color)
{
	mColor = color;
}

void Chunk::SetRebuildFlag()
{
	mRebuildFlag = true;
}

void Chunk::SetChunkIndex(ChunkCoord index)
{
	mChunkIndex = index;
}

void Chunk::SetBlockActive(BlockIndex blockIndex, bool active)
{
	if(blockIndex.x >= 0 && blockIndex.x < CHUNK_SIZE && blockIndex.y >= 0 && blockIndex.y < CHUNK_HEIGHT && blockIndex.z >= 0 && blockIndex.z < CHUNK_SIZE)
	{
		if(mBlocks[blockIndex.x][blockIndex.y][blockIndex.z].IsActive() != active)
			SetRebuildFlag();

		mBlocks[blockIndex.x][blockIndex.y][blockIndex.z].SetActive(active);
	}
	else
	{
		GLIB_ERROR("Invalid block index");
	}
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
	if(mBlockCount == CHUNK_SIZE*CHUNK_HEIGHT*CHUNK_SIZE)
		return;

	// Fill in the front face vertex data.
	ChunkManager::TempChunkVertices[n*24 + 0] = VoxelVertex(x -w2, y -h2, z -d2, 0.0f, 0.0f, -1.0f);
	ChunkManager::TempChunkVertices[n*24 + 1] = VoxelVertex(x -w2, y +h2, z -d2, 0.0f, 0.0f, -1.0f);
	ChunkManager::TempChunkVertices[n*24 + 2] = VoxelVertex(x +w2, y +h2, z -d2, 0.0f, 0.0f, -1.0f);
	ChunkManager::TempChunkVertices[n*24 + 3] = VoxelVertex(x +w2, y -h2, z -d2, 0.0f, 0.0f, -1.0f);

	// Fill in the back face vertex data.
	ChunkManager::TempChunkVertices[n*24 + 4] = VoxelVertex(x -w2, y -h2, z +d2, 0.0f, 0.0f, 1.0f);
	ChunkManager::TempChunkVertices[n*24 + 5] = VoxelVertex(x +w2, y -h2, z +d2, 0.0f, 0.0f, 1.0f);
	ChunkManager::TempChunkVertices[n*24 + 6] = VoxelVertex(x +w2, y +h2, z +d2, 0.0f, 0.0f, 1.0f);
	ChunkManager::TempChunkVertices[n*24 + 7] = VoxelVertex(x -w2, y +h2, z +d2, 0.0f, 0.0f, 1.0f);

	// Fill in the top face vertex data.
	ChunkManager::TempChunkVertices[n*24 + 8]  = VoxelVertex(x -w2, y +h2, z -d2, 0.0f, 1.0f, 0.0f);
	ChunkManager::TempChunkVertices[n*24 + 9]  = VoxelVertex(x -w2, y +h2, z +d2, 0.0f, 1.0f, 0.0f);
	ChunkManager::TempChunkVertices[n*24 + 10] = VoxelVertex(x +w2, y +h2, z +d2, 0.0f, 1.0f, 0.0f);
	ChunkManager::TempChunkVertices[n*24 + 11] = VoxelVertex(x +w2, y +h2, z -d2, 0.0f, 1.0f, 0.0f);

	// Fill in the bottom face vertex data.
	ChunkManager::TempChunkVertices[n*24 + 12] = VoxelVertex(x -w2, y -h2, z -d2, 0.0f, -1.0f, 0.0f);
	ChunkManager::TempChunkVertices[n*24 + 13] = VoxelVertex(x +w2, y -h2, z -d2, 0.0f, -1.0f, 0.0f);
	ChunkManager::TempChunkVertices[n*24 + 14] = VoxelVertex(x +w2, y -h2, z +d2, 0.0f, -1.0f, 0.0f);
	ChunkManager::TempChunkVertices[n*24 + 15] = VoxelVertex(x -w2, y -h2, z +d2, 0.0f, -1.0f, 0.0f);

	// Fill in the left face vertex data.
	ChunkManager::TempChunkVertices[n*24 + 16] = VoxelVertex(x -w2, y -h2, z +d2, -1.0f, 0.0f, 0.0f);
	ChunkManager::TempChunkVertices[n*24 + 17] = VoxelVertex(x -w2, y +h2, z +d2, -1.0f, 0.0f, 0.0f);
	ChunkManager::TempChunkVertices[n*24 + 18] = VoxelVertex(x -w2, y +h2, z -d2, -1.0f, 0.0f, 0.0f);
	ChunkManager::TempChunkVertices[n*24 + 19] = VoxelVertex(x -w2, y -h2, z -d2, -1.0f, 0.0f, 0.0f);

	// Fill in the right face vertex data.
	ChunkManager::TempChunkVertices[n*24 + 20] = VoxelVertex(x +w2, y -h2, z -d2, 1.0f, 0.0f, 0.0f);
	ChunkManager::TempChunkVertices[n*24 + 21] = VoxelVertex(x +w2, y +h2, z -d2, 1.0f, 0.0f, 0.0f);
	ChunkManager::TempChunkVertices[n*24 + 22] = VoxelVertex(x +w2, y +h2, z +d2, 1.0f, 0.0f, 0.0f);
	ChunkManager::TempChunkVertices[n*24 + 23] = VoxelVertex(x +w2, y -h2, z +d2, 1.0f, 0.0f, 0.0f);

	// Fill in the front face index data
	ChunkManager::TempChunkIndices[n*36 + 0] = n*24 + 0; ChunkManager::TempChunkIndices[n*36 + 1] = n*24 + 1; ChunkManager::TempChunkIndices[n*36 + 2] = n*24 + 2;
	ChunkManager::TempChunkIndices[n*36 + 3] = n*24 + 0; ChunkManager::TempChunkIndices[n*36 + 4] = n*24 + 2; ChunkManager::TempChunkIndices[n*36 + 5] = n*24 + 3;

	// Fill in the back face index data
	ChunkManager::TempChunkIndices[n*36 + 6] = n*24 + 4; ChunkManager::TempChunkIndices[n*36 + 7]  = n*24 + 5; ChunkManager::TempChunkIndices[n*36 + 8]  = n*24 + 6;
	ChunkManager::TempChunkIndices[n*36 + 9] = n*24 + 4; ChunkManager::TempChunkIndices[n*36 + 10] = n*24 + 6; ChunkManager::TempChunkIndices[n*36 + 11] = n*24 + 7;

	// Fill in the top face index data
	ChunkManager::TempChunkIndices[n*36 + 12] = n*24 + 8; ChunkManager::TempChunkIndices[n*36 + 13] = n*24 + 9; ChunkManager::TempChunkIndices[n*36 + 14] = n*24 + 10;
	ChunkManager::TempChunkIndices[n*36 + 15] = n*24 + 8; ChunkManager::TempChunkIndices[n*36 + 16] = n*24 + 10; ChunkManager::TempChunkIndices[n*36 + 17] = n*24 + 11;

	// Fill in the bottom face index data
	ChunkManager::TempChunkIndices[n*36 + 18] = n*24 + 12; ChunkManager::TempChunkIndices[n*36 + 19] = n*24 + 13; ChunkManager::TempChunkIndices[n*36 + 20] = n*24 + 14;
	ChunkManager::TempChunkIndices[n*36 + 21] = n*24 + 12; ChunkManager::TempChunkIndices[n*36 + 22] = n*24 + 14; ChunkManager::TempChunkIndices[n*36 + 23] = n*24 + 15;

	// Fill in the left face index data
	ChunkManager::TempChunkIndices[n*36 + 24] = n*24 + 16; ChunkManager::TempChunkIndices[n*36 + 25] = n*24 + 17; ChunkManager::TempChunkIndices[n*36 + 26] = n*24 + 18;
	ChunkManager::TempChunkIndices[n*36 + 27] = n*24 + 16; ChunkManager::TempChunkIndices[n*36 + 28] = n*24 + 18; ChunkManager::TempChunkIndices[n*36 + 29] = n*24 + 19;

	// Fill in the right face index data
	ChunkManager::TempChunkIndices[n*36 + 30] = n*24 + 20; ChunkManager::TempChunkIndices[n*36 + 31] = n*24 + 21; ChunkManager::TempChunkIndices[n*36 + 32] = n*24 + 22;
	ChunkManager::TempChunkIndices[n*36 + 33] = n*24 + 20; ChunkManager::TempChunkIndices[n*36 + 34] = n*24 + 22; ChunkManager::TempChunkIndices[n*36 + 35] = n*24 + 23;

	mBlockCount++;
}