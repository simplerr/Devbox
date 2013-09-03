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
#include "VoxelEffect.h"

GLib::VoxelVertex ChunkManager::TempChunkVertices[24*Chunk::CHUNK_SIZE*Chunk::CHUNK_HEIGHT*Chunk::CHUNK_SIZE];
UINT ChunkManager::TempChunkIndices[36*Chunk::CHUNK_SIZE*Chunk::CHUNK_HEIGHT*Chunk::CHUNK_SIZE];

int quadtree_counter = 0;

GLib::Frustum testFrustum;
XMFLOAT3 testCameraPos;

int ChunkManager::MAX_CHUNKS_LOADED_PER_FRAME = 1;
int ChunkManager::CHUNK_LOAD_RADIUS = 5;
int ChunkManager::WORLD_SIZE_IN_CHUNKS = 32;

ChunkCoord ChunkManager::PlayerChunkCoord = ChunkCoord();

double stopwatch() 
{
	unsigned long long ticks;
	unsigned long long ticks_per_sec;
	QueryPerformanceFrequency( (LARGE_INTEGER *)&ticks_per_sec);
	QueryPerformanceCounter((LARGE_INTEGER *)&ticks);
	return ((float)ticks) / (float)ticks_per_sec;
}

float startTime = stopwatch();

int totalChunksLoaded = 0;

bool ChunkIntersectionCompare(ChunkIntersection a, ChunkIntersection b)
{
	return a.distance < b.distance;
}

bool LoadListCompare(ChunkCoord a, ChunkCoord b)
{
	ChunkCoord diff_a = a - ChunkManager::PlayerChunkCoord;
	ChunkCoord diff_b = b - ChunkManager::PlayerChunkCoord;

	return sqrt(diff_a.x * diff_a.x + diff_a.z *diff_a.z) < sqrt(diff_b.x * diff_b.x + diff_b.z *diff_b.z);
}

ChunkManager::ChunkManager()
{
	// Debug stuff
	testCameraPos = GLib::GlobalApp::GetCamera()->GetPosition();
	testFrustum = GLib::GlobalApp::GetCamera()->GetFrustum();

	mLastChunkId = 0;
	mTestBox = XMFLOAT3(10000, 100, 10000);

	mChunkMap.resize(WORLD_SIZE_IN_CHUNKS*WORLD_SIZE_IN_CHUNKS);

	// Sets the world center where the camera is.
	// [NOTE][TODO] The world center should be hardcoded in later stages.
	mWorldCenterIndex = PositionToChunkCoord(testCameraPos);

	mDrawDebug = false;
}

ChunkManager::~ChunkManager()
{
	Clear();
}

void ChunkManager::Clear()
{
	mChunkMap.clear();
}

bool ChunkManager::IsChunkLoaded(const ChunkId& chunkId)
{
	//if(!InBounds(chunkId))
	//	GLIB_ERROR("Chunk out of bounds!");
	//else
		return mChunkMap[chunkId] != nullptr;
}

int ChunkManager::ChunkCoordToIndex(const ChunkCoord& index)
{
	int x = index.x - mWorldCenterIndex.x + WORLD_SIZE_IN_CHUNKS/2;
	int z = index.z - mWorldCenterIndex.z + WORLD_SIZE_IN_CHUNKS/2;

	return x * WORLD_SIZE_IN_CHUNKS + z;
}

void ChunkManager::Update(float dt)
{
	// Update the list with the chunks to load.
	// Adds chunks in a radius around the camera.
	UpdateLoadList();

	// Load the chunks in the load list.
	LoadChunks();

	// Get the cameras XY positin on the chunk grid.
	PlayerChunkCoord = PositionToChunkCoord(GLib::GlobalApp::GetCamera()->GetPosition());

	if(totalChunksLoaded == CHUNK_LOAD_RADIUS*CHUNK_LOAD_RADIUS*4) 
		float end = stopwatch() - startTime;

	auto input = GLib::GlobalApp::GetInput();

	// TESTING - Rebuild the cameras chunk.
	if(input->KeyPressed('F'))
	{
		mChunkMap[ChunkCoordToIndex(PositionToChunkCoord(GLib::GlobalApp::GetCamera()->GetPosition()))]->BuildSphere();
		mChunkMap[ChunkCoordToIndex(PositionToChunkCoord(GLib::GlobalApp::GetCamera()->GetPosition()))]->Rebuild();
	}

	// TESTING - Store the current camera frustum.
	if(input->KeyPressed('C'))
		testFrustum = GLib::GlobalApp::GetCamera()->GetFrustum();

	if(input->KeyPressed(VK_LBUTTON))
	{
		GLib::Ray ray = GLib::GlobalApp::GetCamera()->GetWorldPickingRay();
		XMVECTOR origin = XMLoadFloat3(&ray.origin);
		XMVECTOR dir = XMVector3Normalize(XMLoadFloat3(&ray.direction));

		// Intersected chunks sorted by distance.
		vector<ChunkIntersection> intersectedChunks;

		// Broadphase just testing AABBs.
		float dist = 99999;
		for(int i = 0; i < mChunkMap.size(); i++)
		{
			if(mChunkMap[i] != nullptr && mChunkMap[i]->RayIntersectBox(origin, dir, dist))
				intersectedChunks.push_back(ChunkIntersection(mChunkMap[i], dist));
		}

		// Sort the intersection array.
		std::sort(intersectedChunks.begin(), intersectedChunks.end(), ChunkIntersectionCompare);

		dist = 99999;
		for(int i = 0; i < intersectedChunks.size(); i++)
		{
			if(intersectedChunks[i].chunk->RayIntersectMesh(origin, dir, dist))
			{
				XMFLOAT3 intersectPosition = ray.origin + dist * ray.direction;

				float add = 0.01f;
				intersectPosition.x += ray.direction.x < 0 ? -add : add;
				intersectPosition.y += ray.direction.y < 0 ? -add : add;
				intersectPosition.z += ray.direction.z < 0 ? -add : add;
				BlockIndex blockIndex = intersectedChunks[i].chunk->PositionToBlockId(intersectPosition);

				intersectedChunks[i].chunk->SetBlockActive(blockIndex, false);
				intersectedChunks[i].chunk->Rebuild();

				// Break the loop since this was the closest chunk.
				// If the mesh ray intersection returns false the next chunk
				// in the intersectedChunks list is tested.
				break;
			}
		}
	}

	// To expensive when the chunk map is so large,
	// since it's filled with nullptrs that needs to be looped over.
	/*for(int i = 0; i < mChunkMap.size(); i++)
	{
		if(mChunkMap[i] != nullptr && mChunkMap[i]->GetRebuildFlag())
			mChunkMap[i]->Rebuild();
	}*/

	// Toggle debug draw information.
	if(input->KeyPressed(VK_F1))
		mDrawDebug = !mDrawDebug;

	float speed = 0.1f;
	if(input->KeyDown(VK_UP))
		mTestBox.x += speed;
	if(input->KeyDown(VK_DOWN))
		mTestBox.x -= speed;
	if(input->KeyDown(VK_LEFT))
		mTestBox.z += speed;
	if(input->KeyDown(VK_RIGHT))
		mTestBox.z -= speed;
}

void ChunkManager::Draw(GLib::Graphics* pGraphics)
{
	quadtree_counter = 0;

	// Do frustum culling using a quadtree to ignore invisible chunks.
	// Do this here in Draw() so we can draw debug information.
	UpdateRenderList();

	// Set the input layout and the primitive topology.
	GLib::GlobalApp::GetD3DContext()->IASetInputLayout(GLib::Effects::VoxelFX->GetInputLayout());
	GLib::GlobalApp::GetD3DContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	XMMATRIX view = XMLoadFloat4x4(&GLib::GlobalApp::GetGraphics()->GetCamera()->GetViewMatrix());
	XMMATRIX proj = XMLoadFloat4x4(&GLib::GlobalApp::GetGraphics()->GetCamera()->GetProjectionMatrix());

	GLib::Effects::VoxelFX->SetWorld(XMMatrixIdentity());
	GLib::Effects::VoxelFX->SetWorldViewProj(XMMatrixIdentity() * view * proj);
	GLib::Effects::VoxelFX->SetColor(GLib::Colors::LightSteelBlue);
	GLib::Effects::VoxelFX->Apply(GLib::GlobalApp::GetD3DContext());

	// Draw all visible chunks.
	for(int i = 0; i < mRenderList.size(); i++)
	{
		mRenderList[i]->Render(pGraphics);

		if(mDrawDebug)
			pGraphics->DrawBoundingBox(&mRenderList[i]->GetAxisAlignedBox(), GLib::Colors::Red, true, 1.0f);
	}

	// Debug information.
	if(mDrawDebug)
		DrawDebug(pGraphics);

	if(GLib::GlobalApp::GetInput()->KeyDown(VK_RBUTTON))
	{
		int height = GetHeight(mTestBox);

		XMFLOAT3 pos = mTestBox;
		pos.y = (height + 1) * Chunk::VOXEL_SIZE;
		pGraphics->DrawBoundingBox(pos, 3, 3, 3);

		// Block height.
		char buffer[128];
		sprintf(buffer, "Height: %i", height);
		pGraphics->DrawText(buffer, 40, 600, 30);
	}
}

/**
	@note Only loads MAX_CHUNKS_LOADED_PER_FRAME chunks per frame.
*/
void ChunkManager::LoadChunks()
{
	int chunksLoaded = 0;
	for(int i = 0; i < mLoadList.size(); i++)
	{
		if(chunksLoaded != MAX_CHUNKS_LOADED_PER_FRAME)
		{
			// Create the chunk.
			StrongChunkPtr chunk = StrongChunkPtr(new Chunk(mLoadList[i].x*Chunk::CHUNK_SIZE*Chunk::VOXEL_SIZE, 0, mLoadList[i].z*Chunk::CHUNK_SIZE*Chunk::VOXEL_SIZE));
			chunk->SetChunkIndex(mLoadList[i]);
			chunk->Init();
			mChunkMap[ChunkCoordToIndex(mLoadList[i])] =  chunk;

			totalChunksLoaded++;
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
	XMFLOAT3 cameraPosition = GLib::GlobalApp::GetCamera()->GetPosition();//testCameraPos
	ChunkCoord playerIndex = PositionToChunkCoord(cameraPosition);

	for(int x = playerIndex.x - CHUNK_LOAD_RADIUS; x < playerIndex.x + CHUNK_LOAD_RADIUS; x++)
	{
		for(int z = playerIndex.z - CHUNK_LOAD_RADIUS; z < playerIndex.z + CHUNK_LOAD_RADIUS; z++)
		{
			// A needed chunk doesn't exist -> load it.
			ChunkCoord chunkIndex = ChunkCoord(x, z);

			ChunkId id = ChunkCoordToIndex(chunkIndex);
			if(InBounds(id) && !IsChunkLoaded(id))//if(mChunkMap.find(chunkIndex) == mChunkMap.end())
			{
				mLoadList.push_back(chunkIndex);
			}
		}
	}

	// Sort the load list by distance from the player.
	if(mLoadList.size() != 0)
		std::sort(mLoadList.begin(), mLoadList.end(), LoadListCompare);
}

void ChunkManager::UpdateRenderList()
{
	mRenderList.clear();

	// Transform camera position to chunk aligned coordinates.
	XMFLOAT3 cameraPos = GLib::GlobalApp::GetCamera()->GetPosition();
	XMFLOAT3 chunkAlignedCameraPos = (ChunkAlignPosition(cameraPos));

	// Traverse a quadtree to quickly find out which chunks are visible.
	TraverseQuadtree(chunkAlignedCameraPos, 32, GLib::GlobalApp::GetCamera()->GetFrustum()); // testFrustum

	//OldFrustumCulling();
}

XMFLOAT3 ChunkManager::ChunkAlignPosition(const XMFLOAT3& position)
{
	float chunkSize = Chunk::CHUNK_SIZE * Chunk::VOXEL_SIZE;
	ChunkCoord tempIndex  = PositionToChunkCoord(position);

	return XMFLOAT3(tempIndex.x * chunkSize, Chunk::CHUNK_HEIGHT * Chunk::VOXEL_SIZE / 2, tempIndex.z * chunkSize);
}

bool ChunkManager::InBounds(const ChunkId& chunkId)
{
	return chunkId >= 0 && chunkId < WORLD_SIZE_IN_CHUNKS*WORLD_SIZE_IN_CHUNKS;
}

void ChunkManager::TraverseQuadtree(const XMFLOAT3& center, int radiusInChunks, const GLib::Frustum& frustum)
{
	quadtree_counter++;

	float chunkSize = Chunk::CHUNK_SIZE * Chunk::VOXEL_SIZE;

	XNA::AxisAlignedBox aabb;
	aabb.Center = center;
	aabb.Extents = XMFLOAT3(chunkSize * radiusInChunks, Chunk::CHUNK_HEIGHT * Chunk::VOXEL_SIZE / 2, chunkSize * radiusInChunks);
	
	// Test AABB collision against the current quad (it's actually a cube but Y is always the same).
	if(frustum.BoxIntersecting(aabb))
	{
		//if(mDrawDebug)
		//	GLib::GlobalApp::GetGraphics()->DrawBoundingBox(&aabb, GLib::Colors::Green, true, 1.0f);

		// Reached the smallest size, now test the remaining 4 chunks individually.
		if(radiusInChunks == 1)
		{
			// Offset to the center of the chunks, to get the ChunkIndex coordinates.
			float offset = chunkSize/2;

			// Add the chunk indexes we need to test AABB collision against.
			// They are calculated from the center of the current quad using an offset.

			// Loop through and test frustum intersection on the chunks.
			StrongChunkPtr chunk = nullptr;

			ChunkId index = ChunkCoordToIndex(PositionToChunkCoord(center.x - offset , center.z - offset));
			if(InBounds(index))
			{
				chunk = mChunkMap[index];
				if(chunk != nullptr && frustum.BoxIntersecting(chunk->GetAxisAlignedBox())) 
					mRenderList.push_back(chunk);
			}

			index = ChunkCoordToIndex(PositionToChunkCoord(center.x + offset , center.z - offset));
			if(InBounds(index))
			{
				chunk = mChunkMap[index];
				if(chunk != nullptr && frustum.BoxIntersecting(chunk->GetAxisAlignedBox())) 
					mRenderList.push_back(chunk);
			}

			index = ChunkCoordToIndex(PositionToChunkCoord(center.x + offset , center.z + offset));
			if(InBounds(index))
			{
				chunk = mChunkMap[index];
				if(chunk != nullptr && frustum.BoxIntersecting(chunk->GetAxisAlignedBox())) 
					mRenderList.push_back(chunk);
			}

			index = ChunkCoordToIndex(PositionToChunkCoord(center.x - offset , center.z + offset));
			if(InBounds(index))
			{
				chunk = mChunkMap[index];
				if(chunk != nullptr && frustum.BoxIntersecting(chunk->GetAxisAlignedBox())) 
					mRenderList.push_back(chunk);
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
		StrongChunkPtr chunk = (*iter);

		if(chunk == nullptr)
			continue;

		// Inside frustum, add to the render list.
		if(GLib::GLibIntersectAxisAlignedBoxFrustum(&chunk->GetAxisAlignedBox(), &frustum))
			mRenderList.push_back(chunk);
	}
}

void ChunkManager::DrawDebug(GLib::Graphics* pGraphics)
{
	// Count total blocks.
	int totalBlocks = 0;
	for(int i = 0; i < mChunkMap.size(); i++)
	{
		if(mChunkMap[i] == nullptr)
			continue;

		totalBlocks += mChunkMap[i]->GetNumBlocks();
	}

	ChunkCoord index = PositionToChunkCoord(GLib::GlobalApp::GetCamera()->GetPosition());
	char buffer[256];
	sprintf(buffer, "Chunk index: [%i]\nChunk coord: [%i][%i]\nTotal blocks: %i\nTrees traversed: %i", ChunkCoordToIndex(index), index.x, index.z, totalBlocks, quadtree_counter);
	pGraphics->DrawText(buffer, 40, 600, 30);

	pGraphics->DrawText("Visible chunks: " + to_string(mRenderList.size()), 10, 500, 40);
}

ChunkCoord ChunkManager::PositionToChunkCoord(XMFLOAT3 position)
{
	ChunkCoord index(0, 0);

	float size = Chunk::CHUNK_SIZE * Chunk::VOXEL_SIZE;

	// Transform to chunk index.
	index.x = position.x / size;
	index.z = position.z / size;

	return index;
}

ChunkCoord ChunkManager::PositionToChunkCoord(float x, float z)
{
	ChunkCoord index(0, 0);

	float size = Chunk::CHUNK_SIZE * Chunk::VOXEL_SIZE;

	// Transform to chunk index.
	index.x = x / size;
	index.z = z / size;

	return index;
}

void ChunkManager::AddVoxel(float x, float y, float z)
{

}

int ChunkManager::GetHeight(XMFLOAT3 position)
{
	int height = 0;

	// Find the Chunk that (x,z) is in.
	ChunkCoord chunkCoord = PositionToChunkCoord(position.x, position.z);
	ChunkId chunkId = ChunkCoordToIndex(chunkCoord);

	if(mChunkMap[chunkId] == nullptr)
		return 0;

	// Find the block id for (x, y, z).
	BlockIndex blockIndex = mChunkMap[chunkId]->PositionToBlockId(XMFLOAT3(position.x, position.y, position.z));

	if(!mChunkMap[chunkId]->IsBlock(blockIndex))
	{
		// Iterate from the current block position downwards to find a block.
		// The first block found is the ground.
		for(int y = blockIndex.y-1; y > 0; y--)
		{
			blockIndex.y = y;

			if(mChunkMap[chunkId]->IsBlock(blockIndex))
			{
				height = blockIndex.y;
				break;
			}
		}
	}
	else
	{
		height = blockIndex.y;
	}

	return height;
}

ChunkId ChunkManager::GetNextChunkId()
{
	++mLastChunkId;
	return mLastChunkId;
}

int ChunkManager::LuaGetHeight(float x, float y, float z)
{
	return GetHeight(XMFLOAT3(x, y, z));
}