#pragma once

#include "Chunk.h"
#include "Frustum.h"
#include <unordered_map>

namespace GLib {
	class Graphics;
	class StaticModel;
}

class Chunk;

typedef shared_ptr<Chunk> StrongChunkPtr;

struct ChunkIntersection
{
	ChunkIntersection(StrongChunkPtr pChunk, float dist)
	{
		chunk = pChunk;
		distance = dist;
	}

	StrongChunkPtr chunk;
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


typedef int ChunkId;

const ChunkId INVALID_CHUNK_ID = 0;

/*
	int GetHeight(float x, float z);
*/

class ChunkManager
{
public:
	ChunkManager();
	~ChunkManager();

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
	ChunkCoord PositionToChunkCoord(XMFLOAT3 position);
	ChunkCoord PositionToChunkCoord(float x, float z);
	StrongChunkPtr PositionToChunk(XMFLOAT3 position);

	int GetHeight(XMFLOAT3 position);

	void AddVoxel(float x, float y, float z);

	void EraseBlock(const GLib::Ray& ray);
	void SpawnBlock(const GLib::Ray& ray);

	// Lua exports.
	int LuaGetHeight(float x, float y, float z);

	static int MAX_CHUNKS_LOADED_PER_FRAME;
	static int CHUNK_LOAD_RADIUS; // Load chunks within this radius from the camera.
	static int WORLD_SIZE_IN_CHUNKS;

	static ChunkCoord PlayerChunkCoord;
private:
	ChunkId GetNextChunkId();
	ChunkId mLastChunkId;

	void TraverseQuadtree(const XMFLOAT3& center, int radiusInChunks, const GLib::Frustum& frustum);
	void OldFrustumCulling();
	XMFLOAT3 ChunkAlignPosition(const XMFLOAT3& position);

	bool IsChunkLoaded(const ChunkId& chunkId);
	bool InBounds(const ChunkId& chunkId);
	int ChunkCoordToIndex(const ChunkCoord& index);
private:
	// All chunks.
	vector<StrongChunkPtr> mChunkMap;

	// Chunks that should be rendered (tested with frustum culling before getting into this list)
	vector<StrongChunkPtr> mRenderList;

	// Chunks that needs to loaded depending on the players (cameras) position.
	vector<ChunkCoord> mLoadList;

	ChunkCoord mWorldCenterIndex;

	XMFLOAT3 mTestBox;

	bool mDrawDebug;
public:
	// Used for building the chunks.
	static GLib::VoxelVertex TempChunkVertices[24*Chunk::CHUNK_SIZE*Chunk::CHUNK_HEIGHT*Chunk::CHUNK_SIZE];
	static UINT TempChunkIndices[36*Chunk::CHUNK_SIZE*Chunk::CHUNK_HEIGHT*Chunk::CHUNK_SIZE];
};

double stopwatch();