#include "GlibStd.h"
#include "ChunkManager.h"
#include "Graphics.h"
#include "ModelImporter.h"
#include "StaticModel.h"
#include "Chunk.h"
#include "Input.h"
#include "Camera.h"
#include "xnacollision.h"
#include <xnamath.h>

GLib::VoxelVertex ChunkManager::TempChunkVertices[24*Chunk::CHUNK_SIZE*Chunk::CHUNK_SIZE*Chunk::CHUNK_SIZE];
UINT ChunkManager::TempChunkIndices[36*Chunk::CHUNK_SIZE*Chunk::CHUNK_SIZE*Chunk::CHUNK_SIZE];

bool ChunkIntersectionCompare(ChunkIntersection a, ChunkIntersection b)
{
	return a.distance < b.distance;
}

ChunkManager::ChunkManager()
{
	mLoadRadius = 4;

	mLastChunkId = 0;
	
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
	UpdateLoadList();
	UpdateRenderList();
	LoadChunks();

	auto input = GLib::GlobalApp::GetInput();
	if(input->KeyPressed('F'))
	{
		mChunkMap[ChunkIndex(0, 0)]->Rebuild();
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

	//ChunkIndex id = PositionToChunkIndex(GLib::GlobalApp::GetCamera()->GetPosition());

	//if(id != INVALID_CHUNK_ID)
	//mChunkMap[id]->SetColor(GLib::Colors::Green);
	
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
	for(int i = 0; i < mRenderList.size(); i++)
	{
		mRenderList[i]->Render(pGraphics);
	}

	pGraphics->DrawText("Visible chunks: " + to_string(mRenderList.size()), 10, 500, 40);

	pGraphics->DrawBoundingBox(mTestBox, 3, 3, 3);

	ChunkIndex index = PositionToChunkIndex(GLib::GlobalApp::GetCamera()->GetPosition());

	char buffer[256];
	sprintf(buffer, "chunk index: [%i][%i]", index.x, index.z);
	pGraphics->DrawText(buffer, 40, 600, 30);

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

void ChunkManager::LoadChunks()
{
	int chunksLoaded = 0;
	for(int i = 0; i < mLoadList.size(); i++)
	{
		if(chunksLoaded != MAX_CHUNKS_LOADED_PER_FRAME)
		{
			// Create the chunk.
			Chunk* chunk = new Chunk(mLoadList[i].x*Chunk::CHUNK_SIZE*Chunk::VOXEL_SIZE, 0, mLoadList[i].z*Chunk::CHUNK_SIZE*Chunk::VOXEL_SIZE);
			chunk->SetChunkIndex(mLoadList[i]);
			chunk->Init();
			mChunkMap[mLoadList[i]] =  chunk;

			chunksLoaded++;
		}
		else
			break;
	}

	mLoadList.clear();
}

void ChunkManager::UpdateLoadList()
{
	// [TEMP][NOTE]
	// Use the camera position for now.
	XMFLOAT3 playerPosition = GLib::GlobalApp::GetCamera()->GetPosition();
	ChunkIndex playerIndex = PositionToChunkIndex(playerPosition);

	// Only add to the load list if it's empty.
	// Otherwise the list will keep on growing and chunks will be loaded twice.
	//if(mLoadList.size() != 0)
	//	return;

	int count = 0;
	for(int x = playerIndex.x - mLoadRadius; x < playerIndex.x + mLoadRadius; x++)
	{
		for(int z = playerIndex.z - mLoadRadius; z < playerIndex.z + mLoadRadius; z++)
		{
			// A needed chunk doesn't exist -> load it.
			ChunkIndex chunkIndex = ChunkIndex(x, z);
			if(mChunkMap.find(chunkIndex) == mChunkMap.end())
			{
				mLoadList.push_back(chunkIndex);
				count++;	
			}
			
		}
	}
	int a = 1;
	// 1.) Find out which chunks that have to be loaded depending on the players position.
	// 2.) Ignore already loaded chunks.

	// Transform the players position to chunk position?

	//mChunkMap[GetNextChunkId()] = new Chunk(i*Chunk::CHUNK_SIZE*Chunk::VOXEL_SIZE, 0, j*Chunk::CHUNK_SIZE*Chunk::VOXEL_SIZE);
}

void ChunkManager::UpdateRenderList()
{
	mRenderList.clear();

	XNA::Frustum frustum = GLib::GlobalApp::GetCamera()->GetFrustum();

	for(auto iter = mChunkMap.begin(); iter != mChunkMap.end(); iter++)
	{
		Chunk* chunk = (*iter).second;
		
		// Inside frustum, add to the render list.
		if(GLib::GLibIntersectAxisAlignedBoxFrustum(&chunk->GetAxisAlignedBox(), &frustum))
		{
			mRenderList.push_back(chunk);
		}
	}
}

ChunkIndex ChunkManager::PositionToChunkIndex(XMFLOAT3 position)
{
	ChunkIndex index(0, 0);

	float size = Chunk::CHUNK_SIZE * Chunk::VOXEL_SIZE;

	// Transform to block position.
	index.x = position.x / size;
	index.z = position.z / size;

	return index;
}

/*ChunkId ChunkManager::PositionToChunkId(XMFLOAT3 position)
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
}*/

void ChunkManager::AddVoxel(float x, float y, float z)
{
	//mVoxelList.push_back(Voxel(x, y, z));
}

ChunkId ChunkManager::GetNextChunkId()
{
	++mLastChunkId;
	return mLastChunkId;
}