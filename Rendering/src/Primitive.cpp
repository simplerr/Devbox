#include "Primitive.h"
#include "Vertex.h"
#include "d3dUtil.h"
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
}
	
void Primitive::SetIndices(ID3D11Device* device, vector<UINT> indices)
{
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
	HR(device->CreateBuffer(&ibd, &initData, &mIndexBuffer));

	mNumIndices = indices.size();
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