#include "ChunkManager.h"
#include "GlibStd.h"
#include "Graphics.h"
#include "ModelImporter.h"
#include "StaticModel.h"
#include "Chunk.h"
#include "Input.h"
#include "Camera.h"

ChunkManager::ChunkManager()
{
	mLastChunkId = 0;

	int size = 5;
	for(int i = 0; i < size; i++)
	{
		for(int j = 0; j < size; j++)
		{
			mChunkMap[GetNextChunkId()] = new Chunk(i*Chunk::CHUNK_SIZE*Chunk::VOXEL_SIZE, 0, j*Chunk::CHUNK_SIZE*Chunk::VOXEL_SIZE);
		}
	}
	
}

void ChunkManager::Clear()
{
	for(auto iter = mChunkMap.begin(); iter != mChunkMap.end(); iter++)
	{
		delete (*iter).second;
	}

	mChunkMap.clear();
}

void ChunkManager::Update(float dt)
{
	if(GLib::GlobalApp::GetInput()->KeyPressed('F'))
	{
		mChunkMap[1]->Rebuild();
	}

	for(auto iter = mChunkMap.begin(); iter != mChunkMap.end(); iter++)
		(*iter).second->SetColor(GLib::Colors::LightSteelBlue);

	ChunkId id = PositionToChunkId(GLib::GlobalApp::GetCamera()->GetPosition());

	if(id != INVALID_CHUNK_ID)
		mChunkMap[id]->SetColor(GLib::Colors::Green);
	
	for(auto iter = mChunkMap.begin(); iter != mChunkMap.end(); iter++)
	{
		GLib::Ray ray = GLib::GlobalApp::GetInput()->GetWorldPickingRay(GLib::GlobalApp::GetCamera());
		XMVECTOR origin = XMLoadFloat3(&ray.origin);
		XMVECTOR dir = XMLoadFloat3(&ray.direction);
		float dist;
		if((*iter).second->RayIntersect(origin, dir, dist))
		{
			(*iter).second->SetColor(GLib::Colors::Red);
			break;
		}
	}
}

void ChunkManager::Draw(GLib::Graphics* pGraphics)
{
	for(auto iter = mChunkMap.begin(); iter != mChunkMap.end(); iter++)
	{
		(*iter).second->Render(pGraphics);
	}
}

ChunkId ChunkManager::PositionToChunkId(XMFLOAT3 position)
{
	for(auto iter = mChunkMap.begin(); iter != mChunkMap.end(); iter++)
	{
		Chunk* chunk = (*iter).second;
		XMFLOAT3 chunkPos = chunk->GetPosition();

		// X
		if(position.x > chunkPos.x && position.x < (chunkPos.x + Chunk::CHUNK_SIZE * Chunk::VOXEL_SIZE))
		{
			// Z
			if(position.z > chunkPos.z && position.z < (chunkPos.z + Chunk::CHUNK_SIZE * Chunk::VOXEL_SIZE))
			{
				return (*iter).first;
			}
		}
	}

	return INVALID_CHUNK_ID;
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