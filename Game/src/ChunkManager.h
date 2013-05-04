#pragma once

#include "Utility.h"
#include "Chunk.h"

namespace GLib {
	class Graphics;
	class StaticModel;
}

class Chunk;

struct ChunkIntersection
{
	ChunkIntersection(Chunk* pChunk, float dist)
	{
		chunk = pChunk;
		distance = dist;
	}

	Chunk* chunk;
	float distance;
};

struct Voxel
{
	Voxel(float x, float y, float z)
	{
		position = XMFLOAT3(x, y, z);
	}

	XMFLOAT3 position;
};


typedef unsigned int ChunkId;
const ChunkId INVALID_CHUNK_ID = 0;

class ChunkManager
{
public:
	ChunkManager();

	void Clear();
	void Update(float dt);
	void Draw(GLib::Graphics* pGraphics);

	void DrawDebug(GLib::Graphics* pGraphics);

	// Does frustum culling on all the chunks.
	void UpdateRenderList();

	// Adds ChunkIndexes for the chunks that needs to be loaded.
	void UpdateLoadList();

	// Loads the chunks in the mLoadList
	void LoadChunks();

	//ChunkId PositionToChunkId(XMFLOAT3 position);
	ChunkIndex PositionToChunkIndex(XMFLOAT3 position);
	ChunkIndex PositionToChunkIndex(float x, float z);

	void AddVoxel(float x, float y, float z);

	static const int MAX_CHUNKS_LOADED_PER_FRAME = 1;
private:
	ChunkId GetNextChunkId();
	ChunkId mLastChunkId;

	void TraverseQuadtree(XMFLOAT3 center, int radiusInChunks, XNA::Frustum& frustum);
	void OldFrustumCulling();
	XMFLOAT3 ChunkAlignPosition(const XMFLOAT3& position);
private:
	// All chunks.
	map<ChunkIndex, Chunk*> mChunkMap;

	// Chunks that should be rendered (tested with frustum culling before getting into this list)
	vector<Chunk*> mRenderList;

	// Chunks that needs to loaded depending on the players (cameras) position.
	vector<ChunkIndex> mLoadList;


	XMFLOAT3 mTestBox;

	// Chunks within this radius from the player should be loaded.
	int mLoadRadius;

	bool mDrawDebug;
public:
	// Used for building the chunks.
	static GLib::VoxelVertex TempChunkVertices[24*Chunk::CHUNK_SIZE*Chunk::CHUNK_SIZE*Chunk::CHUNK_SIZE];
	static UINT TempChunkIndices[36*Chunk::CHUNK_SIZE*Chunk::CHUNK_SIZE*Chunk::CHUNK_SIZE];
};