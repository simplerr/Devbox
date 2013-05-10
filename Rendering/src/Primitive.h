#pragma once

#include <D3DX11.h>
#include <vector>
#include "xnacollision.h"
#include "Vertex.h"
#include "defs.h"

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
	class GLIB_API Primitive
	{
	public:
		template <class VertexType>
		Primitive(ID3D11Device* device, vector<VertexType> vertices, vector<UINT> indices);
		Primitive();
		~Primitive();

		void Cleanup();

		template <class VertexType>
		void SetVertices(ID3D11Device* device, vector<VertexType> vertices, int size);

		template <class VertexType>
		void SetVertices(ID3D11Device* pDevice, VertexType* vertices, int size)
		{
			/******************************************************************************************//**
			* The dynamic vertex buffer. 
			*********************************************************************************************/

			if(mVertexBuffer != nullptr)
				ReleaseCOM(mVertexBuffer);
			if(mStagingVertexBuffer != nullptr)
				ReleaseCOM(mStagingVertexBuffer);

			// Fill out the D3D11_BUFFER_DESC struct.
			D3D11_BUFFER_DESC vbd;
			vbd.Usage = D3D11_USAGE_DYNAMIC;
			vbd.ByteWidth = sizeof(VertexType) * size;
			vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			vbd.MiscFlags = 0;

			// Set the init data.
			D3D11_SUBRESOURCE_DATA initData;
			initData.pSysMem = vertices;

			// Create the vertex buffer.
			HRESULT hr = pDevice->CreateBuffer(&vbd, &initData, &mVertexBuffer);

			/******************************************************************************************//**
			* The staging vertex buffer. 
			*********************************************************************************************/

			// Fill out the D3D11_BUFFER_DESC struct.
			vbd.Usage = D3D11_USAGE_STAGING;	// To let the CPU read from the buffer.
			vbd.ByteWidth = sizeof(VertexType) * size;
			vbd.BindFlags = 0;
			vbd.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
			vbd.MiscFlags = 0;

			// Create the staging vertex buffer we can read from.
			pDevice->CreateBuffer(&vbd, &initData, &mStagingVertexBuffer);

			mNumVertices = size;
		}


		void SetIndices(ID3D11Device* pDevice, vector<UINT> indices);
		void SetIndices(ID3D11Device* pDevice, UINT* indices, int size);

		ID3D11Buffer* GetVertices();
		ID3D11Buffer* GetIndices();
		int NumVertices();
		int NumIndices();
	
		template <class VertexType>
		void Draw(ID3D11DeviceContext* dc);

		// The mapping functions can currently only be used to read from the
		// vertex and index buffers. They use the D3D11_MAP_READ flag.
		// Don't forget to call Unmap().
		template <class VertexType>
		VertexType* MapVertexBuffer();
		UINT* MapIndexBuffer();
		void UnmapVertexBuffer();
		void UnmapIndexBuffer();

		AxisAlignedBox GetBoundingBox();
	private:
		ID3D11Buffer*		  mVertexBuffer;
		ID3D11Buffer*		  mIndexBuffer;
		ID3D11Buffer*		  mStagingVertexBuffer;
		ID3D11Buffer*		  mStagingIndexBuffer;
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
		mVertexBuffer = mStagingVertexBuffer = mIndexBuffer = mStagingIndexBuffer = nullptr;

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
	void Primitive::SetVertices(ID3D11Device* pDevice, vector<VertexType> vertices, int size)
	{
		/******************************************************************************************//**
		* The dynamic vertex buffer. 
		*********************************************************************************************/

		if(mVertexBuffer != nullptr)
			ReleaseCOM(mVertexBuffer);
		if(mStagingVertexBuffer != nullptr)
			ReleaseCOM(mStagingVertexBuffer);

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
		HRESULT hr = pDevice->CreateBuffer(&vbd, &initData, &mVertexBuffer);

		/******************************************************************************************//**
		* The staging vertex buffer. 
		*********************************************************************************************/

		// Fill out the D3D11_BUFFER_DESC struct.
		vbd.Usage = D3D11_USAGE_STAGING;	// To let the CPU read from the buffer.
		vbd.ByteWidth = sizeof(VertexType) * size;
		vbd.BindFlags = 0;
		vbd.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		vbd.MiscFlags = 0;

		// Create the staging vertex buffer we can read from.
		pDevice->CreateBuffer(&vbd, &initData, &mStagingVertexBuffer);

		mNumVertices = size;
	}

	template <class VertexType>
	VertexType* Primitive::MapVertexBuffer()
	{
		// Map the vertex buffer.
		D3D11_MAPPED_SUBRESOURCE vertices_resource;
		GLib::GlobalApp::GetD3DContext()->Map(mStagingVertexBuffer, 0, D3D11_MAP_READ, 0, &vertices_resource);

		return (VertexType*)vertices_resource.pData;
	}
}