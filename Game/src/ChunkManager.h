#pragma once

#include "Utility.h"

namespace GLib {
	class Graphics;
	class StaticModel;
}

class Chunk;

struct Voxel
{
	Voxel(float x, float y, float z)
	{
		position = XMFLOAT3(x, y, z);
	}

	XMFLOAT3 position;
};

typedef unsigned int ChunkId;

class ChunkManager
{
public:
	ChunkManager();

	void Clear();
	void Update(float dt);
	void Draw(GLib::Graphics* pGraphics);

	void PositionToChunk(XMFLOAT3 position);

	void AddVoxel(float x, float y, float z);

private:
	ChunkId GetNextChunkId();
	ChunkId mLastChunkId;

private:
	map<ChunkId, Chunk*> mChunkList;
};