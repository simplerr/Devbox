#pragma once

#include "Vertex.h"
#include "GlibStd.h"
#include "xnacollision.h"
#include "noise\noise.h"

#pragma comment(lib, "../debug/libnoise.lib")

namespace GLib {
	class Graphics;
	class Primitive;
}

enum BlockType
{
	BlockType_Grass = 0,
	BlockType_Dirt,
	BlockType_Water,
	BlockType_Stone,
	BlockType_Wood,
	BlockType_Sand,

	BlockType_NumTypes,
};

// Returned from Chunk::PositionToBlockId().
struct BlockIndex
{
	int x, y, z;
};

class Block
{
public:
	Block() : mActive(false) {

	}

	~Block() {};

	inline const bool& IsActive()	{ return mActive; }
	void SetActive(bool active) { mActive = active; }

	BlockType mBlockType;
	bool mActive;
};

struct ChunkCoord
{
	ChunkCoord() { x = z = 0;}
	ChunkCoord(int x, int z)
	{
		this->x = x;
		this->z = z;
	}

	int x, z;
};

struct ChunkIndexHash
{
	std::size_t operator()(const ChunkCoord& k) const
	{
		// You may want to use a better hash function
		return static_cast<std::size_t>(k.x) ^ static_cast<std::size_t>(k.z);
	}
};

bool operator<(const ChunkCoord a, const ChunkCoord b);
bool operator==(const ChunkCoord& lhs, const ChunkCoord& rhs);
ChunkCoord& operator-(const ChunkCoord& lhs, const ChunkCoord& rhs);
ChunkCoord& operator+(const ChunkCoord& lhs, const ChunkCoord& rhs);

/******************************************************************************************//**
* The position of a chunk is in the "left bottom" corner, not in the center.
* Blocks are added along the positive axes, if the first chunk is at (0, 0, 0) and got 
* CHUNK_SIZE = 16, VOXEL_SIZE = 4 the chunk ranges from (0, 0, 0) - (64, 64, 64)
*********************************************************************************************/

class Chunk
{
public:
	Chunk(float x, float y, float z);
	~Chunk();

	void Init();

	// Loops over all blocks and adds them to the vertex buffer.
	void CreateMesh();

	// Adds a cube to the vertex buffer.
	void AddCube(int x, int y, int z, BlockType type = BlockType::BlockType_Grass);

	void BuildMeshPrimitive();
	void Rebuild();

	void Render(GLib::Graphics* pGraphics);
	bool RayIntersectBox(XMVECTOR origin, XMVECTOR direction, float& pDist);

	// [TODO] This doesn't work longer since the vertices are only temporary stored.
	bool RayIntersectMesh(XMVECTOR origin, XMVECTOR direction, float& pDist);
	void TraverseCollisionOctree(const XMFLOAT3& center, const int& extents);

	void SetColor(XMFLOAT4 color);
	void SetBlockActive(BlockIndex blockIndex, bool active);
	void SetBlockType(BlockIndex blockIndex, BlockType type);
	void SetRebuildFlag();
	void SetChunkIndex(ChunkCoord index);

	XMFLOAT3 GetPosition();
	XNA::AxisAlignedBox GetAxisAlignedBox();
	bool GetRebuildFlag();
	int GetNumBlocks();
	ChunkCoord GetChunkCoord();
	bool IsBlock(const BlockIndex& blockIndex);

	// Returns the index to the block that position is inside.
	BlockIndex PositionToBlockId(XMFLOAT3 position);

	void BuildSphere();
	void BuildLandscape();

	static const int CHUNK_HEIGHT = 128;
	static const int CHUNK_SIZE = 16;
	static const int VOXEL_SIZE = 2;

private:
	bool mBlocks[CHUNK_SIZE][CHUNK_HEIGHT][CHUNK_SIZE];
	int mBlockTypes[CHUNK_SIZE][CHUNK_HEIGHT][CHUNK_SIZE];

	GLib::Primitive* mPrimitive;
	XMFLOAT3 mPosition;
	ChunkCoord mChunkIndex;
	int mBlockCount;
	float mMaxHeight;
	bool mRebuildFlag;

	XMFLOAT4 mColor;

	static noise::module::Perlin perlin;
};

/*
// Adds a cube to the vertex buffer.
void Chunk::AddCube(int x, int y, int z)
{
vector<VoxelVertex> vertices(24);
vector<UINT> indices(36);

// Create the vertices.
float w2, h2, d2;
w2 = h2 = d2 = VOXEL_SIZE;

// Fill in the front face vertex data.
vertices[0] = VoxelVertex(x -w2, y -h2, z -d2);
vertices[1] = VoxelVertex(x -w2, y +h2, z -d2);
vertices[2] = VoxelVertex(x +w2, y +h2, z -d2);
vertices[3] = VoxelVertex(x +w2, y -h2, z -d2);

// Fill in the back face vertex data.
vertices[4] = VoxelVertex(x -w2, y -h2, z +d2);
vertices[5] = VoxelVertex(x +w2, y -h2, z +d2);
vertices[6] = VoxelVertex(x +w2, y +h2, z +d2);
vertices[7] = VoxelVertex(x -w2, y +h2, z +d2);

// Fill in the top face vertex data.
vertices[8]  = VoxelVertex(x -w2, y +h2, z -d2);
vertices[9]  = VoxelVertex(x -w2, y +h2, z +d2);
vertices[10] = VoxelVertex(x +w2, y +h2, z +d2);
vertices[11] = VoxelVertex(x +w2, y +h2, z -d2);

// Fill in the bottom face vertex data.
vertices[12] = VoxelVertex(x -w2, y -h2, z -d2);
vertices[13] = VoxelVertex(x +w2, y -h2, z -d2);
vertices[14] = VoxelVertex(x +w2, y -h2, z +d2);
vertices[15] = VoxelVertex(x -w2, y -h2, z +d2);

// Fill in the left face vertex data.
vertices[16] = VoxelVertex(x -w2, y -h2, z +d2);
vertices[17] = VoxelVertex(x -w2, y +h2, z +d2);
vertices[18] = VoxelVertex(x -w2, y +h2, z -d2);
vertices[19] = VoxelVertex(x -w2, y -h2, z -d2);

// Fill in the right face vertex data.
vertices[20] = VoxelVertex(x +w2, y -h2, z -d2);
vertices[21] = VoxelVertex(x +w2, y +h2, z -d2);
vertices[22] = VoxelVertex(x +w2, y +h2, z +d2);
vertices[23] = VoxelVertex(x +w2, y -h2, z +d2);

// Fill in the front face index data
indices[0] = 0; indices[1] = 1; indices[2] = 2;
indices[3] = 0; indices[4] = 2; indices[5] = 3;

// Fill in the back face index data
indices[6] = 4; indices[7]  = 5; indices[8]  = 6;
indices[9] = 4; indices[10] = 6; indices[11] = 7;

// Fill in the top face index data
indices[12] = 8; indices[13] =  9; indices[14] = 10;
indices[15] = 8; indices[16] = 10; indices[17] = 11;

// Fill in the bottom face index data
indices[18] = 12; indices[19] = 13; indices[20] = 14;
indices[21] = 12; indices[22] = 14; indices[23] = 15;

// Fill in the left face index data
indices[24] = 16; indices[25] = 17; indices[26] = 18;
indices[27] = 16; indices[28] = 18; indices[29] = 19;

// Fill in the right face index data
indices[30] = 20; indices[31] = 21; indices[32] = 22;
indices[33] = 20; indices[34] = 22; indices[35] = 23;

mBlockCount++;

mPrimitive = new GLib::Primitive(GLib::GlobalApp::GetD3DDevice(), vertices, indices);
}*/