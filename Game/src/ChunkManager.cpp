#include "ChunkManager.h"
#include "GlibStd.h"
#include "Graphics.h"
#include "ModelImporter.h"
#include "StaticModel.h"
#include "Chunk.h"
#include "Input.h"
#include "Camera.h"

bool ChunkIntersectionCompare(ChunkIntersection a, ChunkIntersection b)
{
	return a.distance < b.distance;
}

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
	
	mTestBox = XMFLOAT3(0, 0, 0);
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
	auto input = GLib::GlobalApp::GetInput();
	if(input->KeyPressed('F'))
	{
		mChunkMap[1]->Rebuild();
	}

	// Testing ray interesecttion and creating block.
	if(input->KeyPressed(VK_LBUTTON))
	{
		GLib::Ray ray = GLib::GlobalApp::GetCamera()->GetWorldPickingRay();
		XMVECTOR origin = XMLoadFloat3(&ray.origin);
		XMVECTOR dir = XMVector3Normalize(XMLoadFloat3(&ray.direction));

		// Intersected chunks sorted by distance.
		vector<ChunkIntersection> intersectedChunks;

		// Broadphase just testing AABBs.
		float dist = 99999;
		for(auto iter = mChunkMap.begin(); iter != mChunkMap.end(); iter++)
		{
			if((*iter).second->RayIntersectBox(origin, dir, dist))
				intersectedChunks.push_back(ChunkIntersection((*iter).second, dist));
		}

		// Sort the intersection array.
		std::sort(intersectedChunks.begin(), intersectedChunks.end(), ChunkIntersectionCompare);

		for(int i = 0; i < intersectedChunks.size(); i++)
		{
			float dist;
			if(intersectedChunks[i].chunk->RayIntersectMesh(origin, dir, dist))
			{
				XMFLOAT3 intersectPosition = ray.origin + dist * ray.direction;
				float add = 0.01f;
				intersectPosition.x += ray.direction.x < 0 ? -add : add;
				intersectPosition.y += ray.direction.y < 0 ? -add : add;
				intersectPosition.z += ray.direction.z < 0 ? -add : add;
				BlockIndex blockIndex = intersectedChunks[i].chunk->PositionToBlockId(intersectPosition);

				intersectedChunks[i].chunk->SetBlockActive(blockIndex, false);

				// Break the loop since this was the closest chunk.
				// If the mesh ray intersection returns false the next chunk
				// in the intersectedChunks list is tested.
				break;
			}
		}

		
		
	}

	for(auto iter = mChunkMap.begin(); iter != mChunkMap.end(); iter++)
	{
		// Rebuild chunks with the rebuild flag set.
		if((*iter).second->GetRebuildFlag())
			(*iter).second->Rebuild();

		(*iter).second->SetColor(GLib::Colors::LightSteelBlue);
	}

	ChunkId id = PositionToChunkId(GLib::GlobalApp::GetCamera()->GetPosition());

	if(id != INVALID_CHUNK_ID)
		mChunkMap[id]->SetColor(GLib::Colors::Green);
	
	/*for(auto iter = mChunkMap.begin(); iter != mChunkMap.end(); iter++)
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
	}*/

	float speed = 1.0f;
	if(input->KeyDown('W'))
	{
		mTestBox.x -= speed;
	}
	else if(input->KeyDown('S'))
	{
		mTestBox.x += speed;
	}
	else if(input->KeyDown('A'))
	{
		mTestBox.z -= speed;
	}
	else if(input->KeyDown('D'))
	{
		mTestBox.z += speed;
	}
}

void ChunkManager::Draw(GLib::Graphics* pGraphics)
{
	for(auto iter = mChunkMap.begin(); iter != mChunkMap.end(); iter++)
	{
		(*iter).second->Render(pGraphics);
	}

	pGraphics->DrawBoundingBox(mTestBox, 3, 3, 3);

	ChunkId chunkIndex = PositionToChunkId(mTestBox);

	/*if(chunkIndex != INVALID_CHUNK_ID)
	{
		auto blockIndex = mChunkMap[chunkIndex]->PositionToBlockId(mTestBox);
		char buffer[256];
		sprintf(buffer, "chunk index: %i\nblock index: [%i][%i][%i]", chunkIndex, blockIndex.x, blockIndex.y, blockIndex.z);
		pGraphics->DrawText(buffer, 40, 600, 30);

		// Leave blocks after the test box.
		if(blockIndex.x != -1)
		{
			mChunkMap[chunkIndex]->SetBlockActive(blockIndex, true);
		}
	}*/
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