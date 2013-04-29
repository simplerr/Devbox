#include "Chunk.h"
#include <windows.h>
#include "Vertex.h"
#include "GlibStd.h"
#include "Primitive.h"
#include "Effects.h"
#include "VoxelEffect.h"
#include "Graphics.h"
#include "Camera.h"

using namespace GLib;

Chunk::Chunk(float x, float y, float z)
{
	// Create the blocks.
	mBlocks = new Block**[CHUNK_SIZE];
	for(int i = 0; i < CHUNK_SIZE; i++)
	{
		mBlocks[i] = new Block*[CHUNK_SIZE];

		for(int j = 0; j < CHUNK_SIZE; j++)
		{
			mBlocks[i][j] = new Block[CHUNK_SIZE];
		}
	}

	mPosition = XMFLOAT3(x, y, z);

	mBlockCount = 0;

	mPrimitive = nullptr;

	CreateMesh();
	mPrimitive = new GLib::Primitive;
	BuildMeshPrimitive();
}

Chunk::~Chunk()
{
	// Delete the blocks
	for (int i = 0; i < CHUNK_SIZE; ++i)
	{
		for (int j = 0; j < CHUNK_SIZE; ++j)
		{
			delete [] mBlocks[i][j];
		}

		delete [] mBlocks[i];
	}
	delete [] mBlocks;
}

// Loops over all blocks and adds them to the vertex buffer.
void Chunk::CreateMesh()
{
	for(int x = 0; x < CHUNK_SIZE; x++)
	{
		for(int y = 0; y < CHUNK_SIZE; y++)
		{
			for(int z = 0; z < CHUNK_SIZE; z++)
			{
				if(mBlocks[x][y][z].IsActive())
				{
					float X = x - CHUNK_SIZE/2;
					float Y = y - CHUNK_SIZE/2;
					float Z = z - CHUNK_SIZE/2;
					if(rand() % 15 == 0)
						AddCube(mPosition.x + x*VOXEL_SIZE + VOXEL_SIZE/2 , mPosition.y + y*VOXEL_SIZE + VOXEL_SIZE/2, mPosition.z + z*VOXEL_SIZE + VOXEL_SIZE/2);
				}
				else
				{
					continue;
				}
			}
		}
	}
}


void Chunk::BuildMeshPrimitive()
{
	// Only rebuild mBlockCount blocks.
	// They are located at [0] -> [(mBlockCount-1) * 24 or 36] in the vertex and index array.
	mPrimitive->SetVertices(GLib::GlobalApp::GetD3DDevice(), vertices, 24*mBlockCount);
	mPrimitive->SetIndices(GLib::GlobalApp::GetD3DDevice(), indices, 36*mBlockCount);
}

void Chunk::Rebuild()
{
	// Technically no blocks gets removed from the arrays,
	// they are just overwritten. If the new block count is 
	// less then the last one, the old blocks are actually still stored
	// in the arrays, but the buffers don't take them into account due to
	// ByteWidth = sizeof(VoxelVertex) * mBlockCount * 24 and
	// ByteWidth = sizeof(UINT) * mBlockCount * 36

	// Restore the block count.
	mBlockCount = 0;
	CreateMesh();
	BuildMeshPrimitive();
}

void Chunk::Render(GLib::Graphics* pGraphics)
{
	// Set the input layout and the primitive topology.
	GlobalApp::GetD3DContext()->IASetInputLayout(Effects::VoxelFX->GetInputLayout());
	GlobalApp::GetD3DContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	XMMATRIX view = XMLoadFloat4x4(&GLib::GlobalApp::GetGraphics()->GetCamera()->GetViewMatrix());
	XMMATRIX proj = XMLoadFloat4x4(&GLib::GlobalApp::GetGraphics()->GetCamera()->GetProjectionMatrix());

	GLib::Effects::VoxelFX->SetColor(Colors::LightSteelBlue);
	GLib::Effects::VoxelFX->SetWorld(XMMatrixIdentity());
	GLib::Effects::VoxelFX->SetWorldViewProj(XMMatrixIdentity() * view * proj);
	Effects::BasicFX->SetWorldInvTranspose(InverseTranspose(XMMatrixIdentity()));
	GLib::Effects::VoxelFX->Apply(GLib::GlobalApp::GetD3DContext());

	mPrimitive->Draw<VoxelVertex>(GLib::GlobalApp::GetD3DContext());

	float size = CHUNK_SIZE * VOXEL_SIZE;
	float localCenter = size / 2;
	
	pGraphics->DrawBoundingBox(XMFLOAT3(mPosition.x + localCenter, mPosition.y + localCenter, mPosition.z + localCenter), size, size, size, Colors::Green, true, 1.0f);

	pGraphics->DrawBoundingBox(XMFLOAT3(mPosition.x, mPosition.y, mPosition.z), 3, 3, 3, Colors::Black, false, 1.0f);
}

// Adds a cube to the vertex buffer.
void Chunk::AddCube(int x, int y, int z)
{
	// Create the vertices.
	float w2, h2, d2;
	w2 = h2 = d2 = VOXEL_SIZE/2;

	int n = mBlockCount;

	// [HACK] Avoid writing outside the vertices array, don't know why
	// it only happens after a while.
	if(mBlockCount == CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE)
		return;

	// Fill in the front face vertex data.
	vertices[n*24 + 0] = VoxelVertex(x -w2, y -h2, z -d2, 0.0f, 0.0f, -1.0f);
	vertices[n*24 + 1] = VoxelVertex(x -w2, y +h2, z -d2, 0.0f, 0.0f, -1.0f);
	vertices[n*24 + 2] = VoxelVertex(x +w2, y +h2, z -d2, 0.0f, 0.0f, -1.0f);
	vertices[n*24 + 3] = VoxelVertex(x +w2, y -h2, z -d2, 0.0f, 0.0f, -1.0f);

	// Fill in the back face vertex data.
	vertices[n*24 + 4] = VoxelVertex(x -w2, y -h2, z +d2, 0.0f, 0.0f, 1.0f);
	vertices[n*24 + 5] = VoxelVertex(x +w2, y -h2, z +d2, 0.0f, 0.0f, 1.0f);
	vertices[n*24 + 6] = VoxelVertex(x +w2, y +h2, z +d2, 0.0f, 0.0f, 1.0f);
	vertices[n*24 + 7] = VoxelVertex(x -w2, y +h2, z +d2, 0.0f, 0.0f, 1.0f);

	// Fill in the top face vertex data.
	vertices[n*24 + 8]  = VoxelVertex(x -w2, y +h2, z -d2, 0.0f, 1.0f, 0.0f);
	vertices[n*24 + 9]  = VoxelVertex(x -w2, y +h2, z +d2, 0.0f, 1.0f, 0.0f);
	vertices[n*24 + 10] = VoxelVertex(x +w2, y +h2, z +d2, 0.0f, 1.0f, 0.0f);
	vertices[n*24 + 11] = VoxelVertex(x +w2, y +h2, z -d2, 0.0f, 1.0f, 0.0f);

	// Fill in the bottom face vertex data.
	vertices[n*24 + 12] = VoxelVertex(x -w2, y -h2, z -d2, 0.0f, -1.0f, 0.0f);
	vertices[n*24 + 13] = VoxelVertex(x +w2, y -h2, z -d2, 0.0f, -1.0f, 0.0f);
	vertices[n*24 + 14] = VoxelVertex(x +w2, y -h2, z +d2, 0.0f, -1.0f, 0.0f);
	vertices[n*24 + 15] = VoxelVertex(x -w2, y -h2, z +d2, 0.0f, -1.0f, 0.0f);

	// Fill in the left face vertex data.
	vertices[n*24 + 16] = VoxelVertex(x -w2, y -h2, z +d2, -1.0f, 0.0f, 0.0f);
	vertices[n*24 + 17] = VoxelVertex(x -w2, y +h2, z +d2, -1.0f, 0.0f, 0.0f);
	vertices[n*24 + 18] = VoxelVertex(x -w2, y +h2, z -d2, -1.0f, 0.0f, 0.0f);
	vertices[n*24 + 19] = VoxelVertex(x -w2, y -h2, z -d2, -1.0f, 0.0f, 0.0f);

	// Fill in the right face vertex data.
	vertices[n*24 + 20] = VoxelVertex(x +w2, y -h2, z -d2, 1.0f, 0.0f, 0.0f);
	vertices[n*24 + 21] = VoxelVertex(x +w2, y +h2, z -d2, 1.0f, 0.0f, 0.0f);
	vertices[n*24 + 22] = VoxelVertex(x +w2, y +h2, z +d2, 1.0f, 0.0f, 0.0f);
	vertices[n*24 + 23] = VoxelVertex(x +w2, y -h2, z +d2, 1.0f, 0.0f, 0.0f);

	// Fill in the front face index data
	indices[n*36 + 0] = n*24 + 0; indices[n*36 + 1] = n*24 + 1; indices[n*36 + 2] = n*24 + 2;
	indices[n*36 + 3] = n*24 + 0; indices[n*36 + 4] = n*24 + 2; indices[n*36 + 5] = n*24 + 3;

	// Fill in the back face index data
	indices[n*36 + 6] = n*24 + 4; indices[n*36 + 7]  = n*24 + 5; indices[n*36 + 8]  = n*24 + 6;
	indices[n*36 + 9] = n*24 + 4; indices[n*36 + 10] = n*24 + 6; indices[n*36 + 11] = n*24 + 7;

	// Fill in the top face index data
	indices[n*36 + 12] = n*24 + 8; indices[n*36 + 13] = n*24 + 9; indices[n*36 + 14] = n*24 + 10;
	indices[n*36 + 15] = n*24 + 8; indices[n*36 + 16] = n*24 + 10; indices[n*36 + 17] = n*24 + 11;

	// Fill in the bottom face index data
	indices[n*36 + 18] = n*24 + 12; indices[n*36 + 19] = n*24 + 13; indices[n*36 + 20] = n*24 + 14;
	indices[n*36 + 21] = n*24 + 12; indices[n*36 + 22] = n*24 + 14; indices[n*36 + 23] = n*24 + 15;

	// Fill in the left face index data
	indices[n*36 + 24] = n*24 + 16; indices[n*36 + 25] = n*24 + 17; indices[n*36 + 26] = n*24 + 18;
	indices[n*36 + 27] = n*24 + 16; indices[n*36 + 28] = n*24 + 18; indices[n*36 + 29] = n*24 + 19;

	// Fill in the right face index data
	indices[n*36 + 30] = n*24 + 20; indices[n*36 + 31] = n*24 + 21; indices[n*36 + 32] = n*24 + 22;
	indices[n*36 + 33] = n*24 + 20; indices[n*36 + 34] = n*24 + 22; indices[n*36 + 35] = n*24 + 23;

	mBlockCount++;
}
