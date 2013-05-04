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

int quadtree_counter = 0;

XNA::Frustum testFrustum;
XMFLOAT3 testCameraPos;

bool ChunkIntersectionCompare(ChunkIntersection a, ChunkIntersection b)
{
	return a.distance < b.distance;
}

ChunkManager::ChunkManager()
{
	mLastChunkId = 0;
	mTestBox = XMFLOAT3(0, 0, 0);

	mDrawDebug = false;

	// Debug stuff
	testCameraPos = GLib::GlobalApp::GetCamera()->GetPosition();
	testFrustum = GLib::GlobalApp::GetCamera()->GetFrustum();
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
	LoadChunks();

	auto input = GLib::GlobalApp::GetInput();
	if(input->KeyPressed('F'))
	{
		mChunkMap[ChunkIndex(0, 0)]->Rebuild();
	}

	if(input->KeyPressed('C'))
		testFrustum = GLib::GlobalApp::GetCamera()->GetFrustum();

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
		break;
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

	// Toggle debug draw information.
	if(input->KeyPressed(VK_F1))
		mDrawDebug = !mDrawDebug;
}

void ChunkManager::Draw(GLib::Graphics* pGraphics)
{
	quadtree_counter = 0;

	// Do frustum culling using a quadtree to ignore invisible chunks.
	// Do this here in Draw() so we can draw debug information.
	UpdateRenderList();

	for(int i = 0; i < mRenderList.size(); i++)
	{
		mRenderList[i]->Render(pGraphics);
	}

	// Debug information.
	if(mDrawDebug)
	{
		DrawDebug(pGraphics);
	}

	pGraphics->DrawBoundingBox(mTestBox, 3, 3, 3);
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
	XMFLOAT3 cameraPosition = GLib::GlobalApp::GetCamera()->GetPosition();
	ChunkIndex playerIndex = PositionToChunkIndex(cameraPosition);

	for(int x = playerIndex.x - CHUNK_LOAD_RADIUS; x < playerIndex.x + CHUNK_LOAD_RADIUS; x++)
	{
		for(int z = playerIndex.z - CHUNK_LOAD_RADIUS; z < playerIndex.z + CHUNK_LOAD_RADIUS; z++)
		{
			// A needed chunk doesn't exist -> load it.
			ChunkIndex chunkIndex = ChunkIndex(x, z);
			if(mChunkMap.find(chunkIndex) == mChunkMap.end())
			{
				mLoadList.push_back(chunkIndex);
			}
		}
	}
}

void ChunkManager::UpdateRenderList()
{
	mRenderList.clear();

	// Transform camera position to chunk aligned coordinates.
	XMFLOAT3 cameraPos = GLib::GlobalApp::GetCamera()->GetPosition();
	XMFLOAT3 chunkAlignedCameraPos = (ChunkAlignPosition(cameraPos));

	// Traverse a quadtree to quickly find out which chunks are visible.
	TraverseQuadtree(chunkAlignedCameraPos, 16, GLib::GlobalApp::GetCamera()->GetFrustum());

	//OldFrustumCulling();
}

XMFLOAT3 ChunkManager::ChunkAlignPosition(const XMFLOAT3& position)
{
	float chunkSize = Chunk::CHUNK_SIZE * Chunk::VOXEL_SIZE;
	ChunkIndex tempIndex  = PositionToChunkIndex(position);

	return XMFLOAT3(tempIndex.x * chunkSize, chunkSize/2, tempIndex.z * chunkSize);
}

void ChunkManager::TraverseQuadtree(XMFLOAT3 center, int radiusInChunks, XNA::Frustum& frustum)
{
	quadtree_counter++;

	float chunkSize = Chunk::CHUNK_SIZE * Chunk::VOXEL_SIZE;

	XNA::AxisAlignedBox aabb;
	aabb.Center = center;
	aabb.Extents = XMFLOAT3(chunkSize * radiusInChunks, chunkSize/2, chunkSize * radiusInChunks);
	
	// Test AABB collision against the current quad (it's actually a cube but Y is always the same).
	if(GLib::GLibIntersectAxisAlignedBoxFrustum(&aabb, &frustum))
	{
		if(mDrawDebug)
			GLib::GlobalApp::GetGraphics()->DrawBoundingBox(&aabb, GLib::Colors::Green, true, 1.0f);

		// Reached the smallest size, now test the remaining 4 chunks individually.
		if(radiusInChunks == 1)
		{
			// Offset to the center of the chunks, to get the ChunkIndex coordinates.
			float offset = chunkSize/2;

			// Add the chunk indexes we need to test AABB collision against.
			// They are calculated from the center of the current quad using an offset.
			vector<ChunkIndex> indexesToTest;
			indexesToTest.push_back(PositionToChunkIndex(center.x - offset , center.z - offset)); // Chunk 1 (-1, -1)
			indexesToTest.push_back(PositionToChunkIndex(center.x + offset , center.z - offset)); // Chunk 2 (+1, -1)
			indexesToTest.push_back(PositionToChunkIndex(center.x + offset , center.z + offset)); // Chunk 3 (+1, +1)
			indexesToTest.push_back(PositionToChunkIndex(center.x - offset , center.z + offset)); // Chunk 4 (-1, +1)

			// Loop through and test frustum intersection on the chunks.
			Chunk* chunk = nullptr;
			for(int i = 0; i < indexesToTest.size(); i++)
			{
				if(mChunkMap.find(indexesToTest[i]) != mChunkMap.end())	// Must use find since [] inserts a nullptr if the key doesn't exist!
				{
					chunk = mChunkMap[indexesToTest[i]];
					if(chunk != nullptr && GLib::GLibIntersectAxisAlignedBoxFrustum(&chunk->GetAxisAlignedBox(), &frustum))
						mRenderList.push_back(chunk);
				}
			}
		}
		else
		{
			// Traverase the child quads
			float offsetToNewCenter = chunkSize * radiusInChunks/2;
			TraverseQuadtree(center + XMFLOAT3(-offsetToNewCenter, 0, -offsetToNewCenter), radiusInChunks/2, frustum); // Quad 1 (-1, -1)
			TraverseQuadtree(center + XMFLOAT3(+offsetToNewCenter, 0, -offsetToNewCenter), radiusInChunks/2, frustum); // Quad 2 (+1, -1)
			TraverseQuadtree(center + XMFLOAT3(+offsetToNewCenter, 0, +offsetToNewCenter), radiusInChunks/2, frustum); // Quad 3 (+1, +1)
			TraverseQuadtree(center + XMFLOAT3(-offsetToNewCenter, 0, +offsetToNewCenter), radiusInChunks/2, frustum); // Quad 4 (-1, +1)
		}
	}
	else if(mDrawDebug)
	{
		// Bounding box not visible, render red.
		GLib::GlobalApp::GetGraphics()->DrawBoundingBox(&aabb, GLib::Colors::Red, true, 1.0f);
	}
}

void ChunkManager::OldFrustumCulling()
{
	// Old frustum culling, without quadtree.
	XNA::Frustum frustum = GLib::GlobalApp::GetCamera()->GetFrustum();

	for(auto iter = mChunkMap.begin(); iter != mChunkMap.end(); iter++)
	{
		Chunk* chunk = (*iter).second;

		// Inside frustum, add to the render list.
		if(GLib::GLibIntersectAxisAlignedBoxFrustum(&chunk->GetAxisAlignedBox(), &frustum))
			mRenderList.push_back(chunk);
	}
}

void ChunkManager::DrawDebug(GLib::Graphics* pGraphics)
{
	// Count total blocks.
	int totalBlocks = 0;
	for(auto iter = mChunkMap.begin(); iter != mChunkMap.end(); iter++)
		totalBlocks += (*iter).second->GetNumBlocks();

	ChunkIndex index = PositionToChunkIndex(GLib::GlobalApp::GetCamera()->GetPosition());
	char buffer[256];
	sprintf(buffer, "Chunk index: [%i][%i]\nTotal blocks: %i\nTrees traversed: %i", index.x, index.z, totalBlocks, quadtree_counter);
	pGraphics->DrawText(buffer, 40, 600, 30);

	pGraphics->DrawText("Visible chunks: " + to_string(mRenderList.size()), 10, 500, 40);
}

ChunkIndex ChunkManager::PositionToChunkIndex(XMFLOAT3 position)
{
	ChunkIndex index(0, 0);

	float size = Chunk::CHUNK_SIZE * Chunk::VOXEL_SIZE;

	// Transform to chunk index.
	index.x = position.x / size;
	index.z = position.z / size;

	return index;
}

ChunkIndex ChunkManager::PositionToChunkIndex(float x, float z)
{
	ChunkIndex index(0, 0);

	float size = Chunk::CHUNK_SIZE * Chunk::VOXEL_SIZE;

	// Transform to chunk index.
	index.x = x / size;
	index.z = z / size;

	return index;
}

void ChunkManager::AddVoxel(float x, float y, float z)
{

}

ChunkId ChunkManager::GetNextChunkId()
{
	++mLastChunkId;
	return mLastChunkId;
}