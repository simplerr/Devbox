#include "Primitive.h"
#include "Vertex.h"
#include "Utility.h"
#include <vector>

using namespace std;
	
//! Graphics Library namespace.
namespace GLib
{

//! Constructor.
Primitive::Primitive()
{

}

//! Cleanup.
Primitive::~Primitive()
{

}

//! Cleanup.
void Primitive::Cleanup()
{
	// Release the buffers.
	ReleaseCOM(mVertexBuffer);
	ReleaseCOM(mIndexBuffer);
	ReleaseCOM(mStagingVertexBuffer);
	ReleaseCOM(mStagingIndexBuffer);
}
	
void Primitive::SetIndices(ID3D11Device* pDevice, vector<UINT> indices)
{
	/******************************************************************************************//**
	* The immutable index buffer. 
	*********************************************************************************************/

	// Fill out the D3D11_BUFFER_DESC struct.
	D3D11_BUFFER_DESC ibd;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
    ibd.ByteWidth = sizeof(UINT) * indices.size();
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;

	// Set the init data.
    D3D11_SUBRESOURCE_DATA initData;
    initData.pSysMem = &indices[0];

	// Create the index buffer.
	HR(pDevice->CreateBuffer(&ibd, &initData, &mIndexBuffer));

	/******************************************************************************************//**
	* The staging index buffer. 
	*********************************************************************************************/

	// Fill out the D3D11_BUFFER_DESC struct.
	ibd.Usage = D3D11_USAGE_STAGING;	// To let the CPU read from the buffer.
	ibd.ByteWidth = sizeof(UINT) * indices.size();
	ibd.BindFlags = 0;
	ibd.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	ibd.MiscFlags = 0;

	// Create temporary vertex buffer we can read from.
	pDevice->CreateBuffer(&ibd, 0, &mStagingIndexBuffer);


	mNumIndices = indices.size();
}

void Primitive::SetIndices(ID3D11Device* pDevice, UINT* indices, int size)
{
	/******************************************************************************************//**
	* The immutable index buffer. 
	*********************************************************************************************/

	// Fill out the D3D11_BUFFER_DESC struct.
	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * size;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;

	// Set the init data.
	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = indices;

	// Create the index buffer.
	HR(pDevice->CreateBuffer(&ibd, &initData, &mIndexBuffer));

	/******************************************************************************************//**
	* The staging index buffer. 
	*********************************************************************************************/

	// Fill out the D3D11_BUFFER_DESC struct.
	ibd.Usage = D3D11_USAGE_STAGING;	// To let the CPU read from the buffer.
	ibd.ByteWidth = sizeof(UINT) * size;
	ibd.BindFlags = 0;
	ibd.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	ibd.MiscFlags = 0;

	// Create temporary vertex buffer we can read from.
	pDevice->CreateBuffer(&ibd, 0, &mStagingIndexBuffer);

	mNumIndices = size;
}

UINT* Primitive::MapIndexBuffer()
{
	// Copy the resources to the staging vertex buffer.
	GLib::GlobalApp::GetD3DContext()->CopyResource(mStagingIndexBuffer, mIndexBuffer);

	// Map the index buffer.
	D3D11_MAPPED_SUBRESOURCE indices_resource;
	GLib::GlobalApp::GetD3DContext()->Map(mStagingIndexBuffer, 0, D3D11_MAP_READ, 0, &indices_resource);

	return (UINT*)indices_resource.pData;
}

void Primitive::UnmapVertexBuffer()
{
	GLib::GlobalApp::GetD3DContext()->Unmap(mStagingVertexBuffer, 0);
}

void Primitive::UnmapIndexBuffer()
{
	GLib::GlobalApp::GetD3DContext()->Unmap(mStagingIndexBuffer, 0);
}

//! Returns the bounding box in local space.
AxisAlignedBox Primitive::GetBoundingBox()
{
	return mBoundingBox;
}

ID3D11Buffer* Primitive::GetVertices()
{
	return mVertexBuffer;
}

ID3D11Buffer* Primitive::GetIndices()
{
	return mIndexBuffer;
}

int Primitive::NumVertices()
{
	return mNumVertices;
}
	
int Primitive::NumIndices()
{
	return mNumIndices;
}

}	// End of Graphics Library namespace.