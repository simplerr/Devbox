#pragma once

#include <D3DX11.h>
#include <vector>
#include "xnacollision.h"
#include "Vertex.h"

using namespace std;
using namespace XNA;

//! Graphics Library namespace.
namespace GLib
{
	// Forward declarations.
	struct Vertex;

	//!
	//	Contains a vertex and index buffer.
	//!
	class Primitive
	{
	public:
		template <class VertexType>
		Primitive(ID3D11Device* device, vector<VertexType> vertices, vector<UINT> indices);
		Primitive();
		~Primitive();

		void Cleanup();

		template <class VertexType>
		void SetVertices(ID3D11Device* device, vector<VertexType> vertices, int size);
		void SetIndices(ID3D11Device* device, vector<UINT> indices);
		ID3D11Buffer* GetVertices();
		ID3D11Buffer* GetIndices();
		int NumVertices();
		int NumIndices();
	
		template <class VertexType>
		void Draw(ID3D11DeviceContext* dc);

		AxisAlignedBox GetBoundingBox();
	private:
		ID3D11Buffer*		  mVertexBuffer;
		ID3D11Buffer*		  mIndexBuffer;
		AxisAlignedBox		  mBoundingBox;
		UINT mNumVertices;
		UINT mNumIndices;
	};

	/**
		Template functions has to be in the header. 
	*/
	template <class VertexType>
	Primitive::Primitive(ID3D11Device* device, vector<VertexType> vertices, vector<UINT> indices)
	{
		SetVertices(device, vertices, vertices.size());
		SetIndices(device, indices);
	}

	template <class VertexType>
	void Primitive::Draw(ID3D11DeviceContext* dc)
	{
		UINT stride = sizeof(VertexType);
		UINT offset = 0;
		dc->IASetVertexBuffers(0, 1, &mVertexBuffer, &stride, &offset);
		dc->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);	// [NOTE] Important with right format!! 16 or 32!
		dc->DrawIndexed(mNumIndices, 0, 0);
	}

	template <class VertexType>
	void Primitive::SetVertices(ID3D11Device* device, vector<VertexType> vertices, int size)
	{
		// Fill out the D3D11_BUFFER_DESC struct.
		D3D11_BUFFER_DESC vbd;
		vbd.Usage = D3D11_USAGE_DYNAMIC;
		vbd.ByteWidth = sizeof(VertexType) * size;
		vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		vbd.MiscFlags = 0;

		// Set the init data.
		D3D11_SUBRESOURCE_DATA initData;
		initData.pSysMem = &vertices[0];

		// Create the vertex buffer.
		HRESULT hr = device->CreateBuffer(&vbd, &initData, &mVertexBuffer);

		mNumVertices = size;

		// Compute the AABB.
		XNA::ComputeBoundingAxisAlignedBoxFromPoints(&mBoundingBox, size, &vertices[0].Pos, sizeof(VertexType));
	}
}