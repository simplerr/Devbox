#include "ChunkManager.h"
#include "GlibStd.h"
#include "Graphics.h"
#include "ModelImporter.h"
#include "StaticModel.h"
#include "Chunk.h"
#include "Input.h"

ChunkManager::ChunkManager()
{
	mLastChunkId = 0;

	int size = 5;
	for(int i = 0; i < size; i++)
	{
		for(int j = 0; j < size; j++)
		{
			mChunkList[GetNextChunkId()] = new Chunk(i*Chunk::CHUNK_SIZE*Chunk::VOXEL_SIZE, 0, j*Chunk::CHUNK_SIZE*Chunk::VOXEL_SIZE);
		}
	}
	
}

void ChunkManager::Clear()
{
	for(auto iter = mChunkList.begin(); iter != mChunkList.end(); iter++)
	{
		delete (*iter).second;
	}

	mChunkList.clear();
}

void ChunkManager::Update(float dt)
{
	if(GLib::GlobalApp::GetInput()->KeyPressed('F'))
	{
		mChunkList[1]->Rebuild();
	}
}

void ChunkManager::Draw(GLib::Graphics* pGraphics)
{
	for(auto iter = mChunkList.begin(); iter != mChunkList.end(); iter++)
	{
		(*iter).second->Render(pGraphics);
	}
}

void ChunkManager::AddVoxel(float x, float y, float z)
{
	//mVoxelList.push_back(Voxel(x, y, z));
}

ChunkId ChunkManager::GetNextChunkId()
{
	++mLastChunkId;
	return mLastChunkId;
}