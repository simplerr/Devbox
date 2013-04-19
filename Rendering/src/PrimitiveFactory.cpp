#include "PrimitiveFactory.h"
#include "Utility.h"
#include "Runnable.h"
#include "D3DCore.h"
#include <D3DX11.h>
#include <vector>
#include "Primitive.h"
#include "Vertex.h"
#include "Graphics.h"
#include "Terrain.h"

//! Graphics Library namespace.
namespace GLib
{

//! Constructor.
PrimitiveFactory::PrimitiveFactory()
{

}
	
PrimitiveFactory::~PrimitiveFactory()
{
	// Cleanup all the primitives.
	for(auto iter = mPrimitiveMap.begin(); iter != mPrimitiveMap.end(); iter++) {
		(*iter).second->Cleanup();
		delete (*iter).second;
	}
}

//! Adds a primitive to the map.
void PrimitiveFactory::AddPrimitive(string name, Primitive* primitive)
{
	mPrimitiveMap[name] = primitive;
}

//! Creates a box.
Primitive* PrimitiveFactory::CreateBox()
{
	// Already created a box primitive?
	if(mPrimitiveMap.find("box") != mPrimitiveMap.end())
		return mPrimitiveMap["box"];

	vector<Vertex> vertices(24);
	vector<UINT> indices(36);

	// Create the vertices.
	float w2, h2, d2;
	w2 = h2 = d2 = 1.0f;

	// Fill in the front face vertex data.
	vertices[0] = Vertex(-w2, -h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	vertices[1] = Vertex(-w2, +h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	vertices[2] = Vertex(+w2, +h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	vertices[3] = Vertex(+w2, -h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

	// Fill in the back face vertex data.
	vertices[4] = Vertex(-w2, -h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	vertices[5] = Vertex(+w2, -h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	vertices[6] = Vertex(+w2, +h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	vertices[7] = Vertex(-w2, +h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	// Fill in the top face vertex data.
	vertices[8]  = Vertex(-w2, +h2, -d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	vertices[9]  = Vertex(-w2, +h2, +d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	vertices[10] = Vertex(+w2, +h2, +d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	vertices[11] = Vertex(+w2, +h2, -d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

	// Fill in the bottom face vertex data.
	vertices[12] = Vertex(-w2, -h2, -d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	vertices[13] = Vertex(+w2, -h2, -d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	vertices[14] = Vertex(+w2, -h2, +d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	vertices[15] = Vertex(-w2, -h2, +d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	// Fill in the left face vertex data.
	vertices[16] = Vertex(-w2, -h2, +d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	vertices[17] = Vertex(-w2, +h2, +d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	vertices[18] = Vertex(-w2, +h2, -d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	vertices[19] = Vertex(-w2, -h2, -d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

	// Fill in the right face vertex data.
	vertices[20] = Vertex(+w2, -h2, -d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
	vertices[21] = Vertex(+w2, +h2, -d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
	vertices[22] = Vertex(+w2, +h2, +d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);
	vertices[23] = Vertex(+w2, -h2, +d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);

	// Fill in the front face index data
	indices[0] = 0; indices[1] = 1; indices[2] = 2;
	indices[3] = 0; indices[4] = 2; indices[5] = 3;

	// Fill in the back face index data
	indices[6] = 4; indices[7]  = 5; indices[8]  = 6;
	indices[9] = 4; indices[10] = 6; indices[11] = 7;

	// Fill in the top face index data
	indices[12] = 8; indices[13] =  9; indices[14] = 10;
	indices[15] = 8; indices[16] = 10; indices[17] = 11;

	// Fill in the bottom face index data
	indices[18] = 12; indices[19] = 13; indices[20] = 14;
	indices[21] = 12; indices[22] = 14; indices[23] = 15;

	// Fill in the left face index data
	indices[24] = 16; indices[25] = 17; indices[26] = 18;
	indices[27] = 16; indices[28] = 18; indices[29] = 19;

	// Fill in the right face index data
	indices[30] = 20; indices[31] = 21; indices[32] = 22;
	indices[33] = 20; indices[34] = 22; indices[35] = 23;

	// Add to the primitive map.
	AddPrimitive("box", new Primitive(GlobalApp::GetD3DDevice(), vertices, indices));
	return mPrimitiveMap["box"];
}

//! Creates a quad.
Primitive* PrimitiveFactory::CreateQuad()
{
	// Already created a box primitive?
	if(mPrimitiveMap.find("quad") != mPrimitiveMap.end())
		return mPrimitiveMap["quad"];

	vector<Vertex> vertices(4);
	vertices[0] = Vertex(-1.0f, -1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	vertices[1] = Vertex(-1.0f, +1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	vertices[2] = Vertex(+1.0f, +1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	vertices[3] = Vertex(+1.0f, -1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

	vector<UINT> indices(6);
	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	indices[3] = 0;
	indices[4] = 2;
	indices[5] = 3;

	// Add to the primitive map.
	AddPrimitive("quad", new Primitive(GlobalApp::GetD3DDevice(), vertices, indices));
	return mPrimitiveMap["quad"];
}

//! Creates a grid.
Primitive* PrimitiveFactory::CreateGrid(float width, float depth, UINT m, UINT n)
{
	if(mPrimitiveMap.find("grid") != mPrimitiveMap.end())
		return mPrimitiveMap["grid"];

	UINT vertexCount = m*n;
	UINT faceCount   = (m-1)*(n-1)*2;

	float halfWidth = 0.5f*width;
	float halfDepth = 0.5f*depth;

	float dx = width / (n-1);
	float dz = depth / (m-1);

	float du = 1.0f / (n-1);
	float dv = 1.0f / (m-1);

	// Create the vertices.
	vector<Vertex> vertices(vertexCount);
	for(UINT i = 0; i < m; ++i)
	{
		float z = halfDepth - i*dz;
		for(UINT j = 0; j < n; ++j)
		{
			float x = -halfWidth + j*dx;

			vertices[i*n+j].Pos		= XMFLOAT3(x, 0.0f, z);
			vertices[i*n+j].Normal	= XMFLOAT3(0, 1, 0);
			vertices[i*n+j].Tangent = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
			vertices[i*n+j].Tex.x	= j*du;
			vertices[i*n+j].Tex.y	= i*dv;
		}
	}

	// Create the indices.
	vector<UINT> indices(faceCount*3);

	// Iterate over each quad and compute indices.
	UINT k = 0;
	for(UINT i = 0; i < m-1; ++i)
	{
		for(UINT j = 0; j < n-1; ++j)
		{
			indices[k]   = i*n+j;
			indices[k+1] = i*n+j+1;
			indices[k+2] = (i+1)*n+j;

			indices[k+3] = (i+1)*n+j;
			indices[k+4] = i*n+j+1;
			indices[k+5] = (i+1)*n+j+1;

			k += 6; // next quad
		}
	}

	// Add to the primitive map.
	AddPrimitive("grid", new Primitive(GlobalApp::GetD3DDevice(), vertices, indices));
	return mPrimitiveMap["grid"];
}

//! Creates a terrain.
Primitive* PrimitiveFactory::CreateTerrain(Terrain* terrain)
{
	string name = terrain->GetInfo().HeightMap;
	if(mPrimitiveMap.find(name) != mPrimitiveMap.end())
		return mPrimitiveMap[name];

	InitInfo info = terrain->GetInfo();

	UINT m = info.HeightmapWidth;
	UINT n = info.HeightmapHeight;
	UINT vertexCount = m * n;
	UINT faceCount   = (m-1)*(n-1)*2;

	float halfWidth = 0.5f * terrain->GetWidth();
	float halfDepth = 0.5f * terrain->GetDepth();

	float dx = info.CellSpacing;
	float dz = info.CellSpacing;

	float du = 1.0f / (m-1);
	float dv = 1.0f / (n-1);

	// Create the vertices.
	vector<Vertex> vertices(vertexCount);
	for(UINT i = 0; i < m; ++i)
	{
		float z = halfDepth - i*dz;
		for(UINT j = 0; j < n; ++j)
		{
			float x = -halfWidth + j*dx;

			vertices[i*n+j].Pos		= XMFLOAT3(x, 0, z);
			vertices[i*n+j].Normal	= XMFLOAT3(0, 1, 0);
			vertices[i*n+j].Tangent = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
			vertices[i*n+j].Tex.x	= j*du;
			vertices[i*n+j].Tex.y	= i*dv;
		}
	}

	// Create the indices.
	vector<UINT> indices(faceCount*3);

	// Iterate over each quad and compute indices.
	UINT k = 0;
	for(UINT i = 0; i < m-1; ++i)
	{
		for(UINT j = 0; j < n-1; ++j)
		{
			indices[k]   = i*n+j;
			indices[k+1] = i*n+j+1;
			indices[k+2] = (i+1)*n+j;

			indices[k+3] = (i+1)*n+j;
			indices[k+4] = i*n+j+1;
			indices[k+5] = (i+1)*n+j+1;

			k += 6; // next quad
		}
	}

	// Add to the primitive map.
	AddPrimitive(name, new Primitive(GlobalApp::GetD3DDevice(), vertices, indices));
	return mPrimitiveMap[name];
}

// From Lunas book.
Primitive* PrimitiveFactory::CreateSphere(float radius, UINT sliceCount, UINT stackCount)
{
	if(mPrimitiveMap.find("sphere") != mPrimitiveMap.end())
		return mPrimitiveMap["sphere"];

	vector<Vertex> vertices;
	vector<UINT> indices;

	// Poles: note that there will be texture coordinate distortion as there is
	// not a unique point on the texture map to assign to the pole when mapping
	// a rectangular texture onto a sphere.
	Vertex topVertex(0.0f, +radius, 0.0f, 0.0f, +1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	Vertex bottomVertex(0.0f, -radius, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);

	vertices.push_back( topVertex );

	float phiStep   = XM_PI/stackCount;
	float thetaStep = 2.0f*XM_PI/sliceCount;

	// Compute vertices for each stack ring (do not count the poles as rings).
	for(UINT i = 1; i <= stackCount-1; ++i)
	{
		float phi = i*phiStep;

		// Vertices of ring.
		for(UINT j = 0; j <= sliceCount; ++j)
		{
			float theta = j*thetaStep;

			Vertex v;

			// spherical to cartesian
			v.Pos.x = radius*sinf(phi)*cosf(theta);
			v.Pos.y = radius*cosf(phi);
			v.Pos.z = radius*sinf(phi)*sinf(theta);

			// Partial derivative of P with respect to theta
			/*v.TangentU.x = -radius*sinf(phi)*sinf(theta);
			v.TangentU.y = 0.0f;
			v.TangentU.z = +radius*sinf(phi)*cosf(theta);

			XMVECTOR T = XMLoadFloat3(&v.TangentU);
			XMStoreFloat3(&v.TangentU, XMVector3Normalize(T));*/

			XMVECTOR p = XMLoadFloat3(&v.Pos);
			XMStoreFloat3(&v.Normal, XMVector3Normalize(p));

			v.Tex.x = theta / XM_2PI;
			v.Tex.y = phi / XM_PI;

			vertices.push_back( v );
		}
	}

	vertices.push_back( bottomVertex );

	//
	// Compute indices for top stack.  The top stack was written first to the vertex buffer
	// and connects the top pole to the first ring.
	//

	for(UINT i = 1; i <= sliceCount; ++i)
	{
		indices.push_back(0);
		indices.push_back(i+1);
		indices.push_back(i);
	}
	
	//
	// Compute indices for inner stacks (not connected to poles).
	//

	// Offset the indices to the index of the first vertex in the first ring.
	// This is just skipping the top pole vertex.
	UINT baseIndex = 1;
	UINT ringVertexCount = sliceCount+1;
	for(UINT i = 0; i < stackCount-2; ++i)
	{
		for(UINT j = 0; j < sliceCount; ++j)
		{
			indices.push_back(baseIndex + i*ringVertexCount + j);
			indices.push_back(baseIndex + i*ringVertexCount + j+1);
			indices.push_back(baseIndex + (i+1)*ringVertexCount + j);

			indices.push_back(baseIndex + (i+1)*ringVertexCount + j);
			indices.push_back(baseIndex + i*ringVertexCount + j+1);
			indices.push_back(baseIndex + (i+1)*ringVertexCount + j+1);
		}
	}

	//
	// Compute indices for bottom stack.  The bottom stack was written last to the vertex buffer
	// and connects the bottom pole to the bottom ring.
	//

	// South pole vertex was added last.
	UINT southPoleIndex = (UINT)vertices.size()-1;

	// Offset the indices to the index of the first vertex in the last ring.
	baseIndex = southPoleIndex - ringVertexCount;
	
	for(UINT i = 0; i < sliceCount; ++i)
	{
		indices.push_back(southPoleIndex);
		indices.push_back(baseIndex+i);
		indices.push_back(baseIndex+i+1);
	}

	// Add to the primitive map.
	AddPrimitive("sphere", new Primitive(GlobalApp::GetD3DDevice(), vertices, indices));
	return mPrimitiveMap["sphere"];
}

}	// End of Graphics Library namespace.