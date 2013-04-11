#include <fstream>
#include <sstream>
#include <algorithm>
#include "Terrain.h"
#include "PrimitiveFactory.h"
#include "Graphics.h"
#include "Effects.h"
#include "Camera.h"
#include "Primitive.h"
#include "ShadowMap.h"
#include <stdint.h>

//! Graphics Library namespace.
namespace GLib
	{
	//! Constructor.
	Terrain::Terrain()
		: mPrimitive(0), mLayerTextureArraySRV(0), mBlendMapSRV(0), mHeightMapSRV(0)
	{
		// Identity matrix.
		XMStoreFloat4x4(&mWorldMatrix, XMMatrixIdentity());
	}
	
	//! Cleanup.
	Terrain::~Terrain()
	{
		ReleaseCOM(mLayerTextureArraySRV);
		ReleaseCOM(mBlendMapSRV);
		ReleaseCOM(mHeightMapSRV);
	}

	//! Loads the heightmap, builds all the SRVs.
	void Terrain::Init(ID3D11Device* device, ID3D11DeviceContext* context, PrimitiveFactory* pPrimitiveFactory, const InitInfo& initInfo)
	{
		mInfo = initInfo;

		// Build the terrain primitive.
		mPrimitive = pPrimitiveFactory->CreateTerrain(this);

		// Create the SRV to the texture array.
		vector<string> layerFilenames;
		layerFilenames.push_back(mInfo.LayerMap0);
		layerFilenames.push_back(mInfo.LayerMap1);
		layerFilenames.push_back(mInfo.LayerMap2);
		layerFilenames.push_back(mInfo.LayerMap3);
		layerFilenames.push_back(mInfo.LayerMap4);
		mLayerTextureArraySRV = d3dHelper::CreateTexture2DArraySRV(device, context, layerFilenames);
	}
	
	//! Draw the terrain with texture blending and shadowmapping.
	void Terrain::Draw(Graphics* pGraphics)
	{
		ID3D11DeviceContext* context = pGraphics->GetContext();

		// Set the input layout and the primitive topology.
		context->IASetInputLayout(Effects::TerrainFX->GetInputLayout());
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// Set all effect variables.
		Camera* camera = pGraphics->GetCamera();
		XMMATRIX view = XMLoadFloat4x4(&camera->GetViewMatrix());
		XMMATRIX proj = XMLoadFloat4x4(&camera->GetProjectionMatrix());
		Effects::TerrainFX->SetViewProj(XMMatrixMultiply(view, proj));
		Effects::TerrainFX->SetEyePosW(camera->GetPosition());
		Effects::TerrainFX->SetLights(pGraphics->GetLightList());
		Effects::TerrainFX->SetFogColor(XMLoadFloat4(&pGraphics->GetFogColor()));
		Effects::TerrainFX->SetFogStart(1000.0f);
		Effects::TerrainFX->SetFogRange(50.0f);
		Effects::TerrainFX->SetMaterial(Material(Colors::White));
		Effects::TerrainFX->SetTextureScale(20.0f);
		Effects::TerrainFX->SetTexelCellSpaceU(1.0f / mInfo.HeightmapWidth);
		Effects::TerrainFX->SetTexelCellSpaceV(1.0f / mInfo.HeightmapHeight);
		Effects::TerrainFX->SetWorldCellSpace(mInfo.CellSpacing);

		//// The blend texture SRVs..
		Effects::TerrainFX->SetLayerMapArray(mLayerTextureArraySRV);
		Effects::TerrainFX->SetBlendMap(mBlendMapSRV);
		Effects::TerrainFX->SetHeightMap(mHeightMapSRV);

		// Set the shadow map and the shadow transform.
		ShadowMap* shadowMap = pGraphics->GetShadowMap();
		Effects::TerrainFX->SetShadowMap(shadowMap->GetSRV());
		Effects::TerrainFX->SetShadowTransform(XMLoadFloat4x4(&shadowMap->GetShadowTransform()));
	
		// Apply them.
		Effects::TerrainFX->Apply(GlobalApp::GetD3DContext());

		// Draw the primitive.
		mPrimitive->Draw<Vertex>(pGraphics->GetContext());
	}

	//! Saves the heightmap.
	void Terrain::SaveHeightMap(ofstream& fout)
	{
		fout << "\n----------------------------HEIGHT_MAP----------------------------" << endl;
		fout << mHeightMap.size() << " ";
		for(int i = 0; i < mHeightMap.size(); i++)
			fout << mHeightMap[i] << " ";
	}

	//! Saves the blendmap.
	void Terrain::SaveBlendMap(ofstream& fout)
	{
		fout << "\n----------------------------BLEND_MAP----------------------------" << endl;
		fout << mBlendMap.size() << " ";
		for(int i = 0; i < mBlendMap.size(); i++)
			fout << mBlendMap[i].x << " " << mBlendMap[i].y << " " << mBlendMap[i].z << " " << mBlendMap[i].w << " ";
	}

	//! Loads a heightmap from a .hmap file.
	void Terrain::LoadHeightmap(ifstream& fin)
	{
		mHeightMap.clear();

		string ignore;
		fin >> ignore;	// -HEIGHT_MAP-

		int size;
		fin >> size;
		for(int i = 0; i < size; i++) {
			float height;
			fin >> height;
			mHeightMap.push_back(height);		
		}

		// Build the heightmap SRV.
		BuildHeightmapSRV(GLib::GlobalApp::GetD3DDevice());
	}

	//! Loads a blendmap from a .bmap file.
	void Terrain::LoadBlendMap(ifstream& fin)
	{
		mBlendMap.clear();

		string ignore;
		fin >> ignore;	// -BLEND_MAP-

		int size;
		fin >> size;
		for(int i = 0; i < size; i++) {
			XMFLOAT4 blend;
			fin >> blend.x >> blend.y >> blend.z >> blend.w;

			// Limit to 0-1 range.
			XMVECTOR limited = XMVectorMax(XMLoadFloat4(&blend), XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f));
			XMStoreFloat4(&blend, limited);
			limited = XMVectorMin(XMLoadFloat4(&blend), XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f));
			XMStoreFloat4(&blend, limited);

			mBlendMap.push_back(blend);
		}

		// Build the blend map SRV.
		BuildBlendMapSRV(GLib::GlobalApp::GetD3DDevice());
	}

	//! Smooths out the heightmap by averaging nearby heights. 
	void Terrain::Smooth()
	{
		std::vector<float> dest(mHeightMap.size());

		for(UINT i = 0; i < mInfo.HeightmapHeight; ++i)
		{
			for(UINT j = 0; j < mInfo.HeightmapWidth; ++j)
			{
				dest[i*mInfo.HeightmapWidth+j] = Average(i,j);
			}
		}

		// Replace the old heightmap with the filtered one.
		mHeightMap = dest;
	}

	//! Smooths out the heightmap in a radius around origin.
	void Terrain::Smooth(XMFLOAT3 origin, int radius)
	{
		// Transform from terrain local space to "cell" space.
		float c = (origin.x + 0.5f*GetWidth()) /  mInfo.CellSpacing;
		float d = (origin.z - 0.5f*GetDepth()) / -mInfo.CellSpacing;

		// Get the row and column we are in.
		int row = (int)floorf(d);
		int col = (int)floorf(c);

		std::vector<float> dest(mHeightMap.size());
		dest = mHeightMap;
		for(int i = row-radius; i < row+radius; ++i)
		{
			for(int j = col-radius; j < col+radius; ++j)
			{
				if(InBounds(i, j))
					dest[i*mInfo.HeightmapWidth+j] = Average(i,j);
			}
		}

		// Replace the old heightmap with the filtered one.
		mHeightMap = dest;
	}

	//! Returns true if (i, j) is inside the heightmap.
	bool Terrain::InBounds(int i, int j)
	{
		// True if ij are valid indices; false otherwise.
		return i >= 0 && i < (int)mInfo.HeightmapHeight && j >= 0 && j < (int)mInfo.HeightmapWidth;
	}
	
	//! Returns the average height for (i, j) in 3x3.
	float Terrain::Average(int i, int j)
	{
		// Function computes the average height of the ij element.
		// It averages itself with its eight neighbor pixels.  Note
		// that if a pixel is missing neighbor, we just don't include it
		// in the average--that is, edge pixels don't have a neighbor pixel.
		//
		// ----------
		// | 1| 2| 3|
		// ----------
		// |4 |ij| 6|
		// ----------
		// | 7| 8| 9|
		// ----------

		float avg = 0.0f;
		float num = 0.0f;

		// Use int to allow negatives.  If we use UINT, @ i=0, m=i-1=UINT_MAX
		// and no iterations of the outer for loop occur.
		for(int m = i-1; m <= i+1; ++m)
		{
			for(int n = j-1; n <= j+1; ++n)
			{
				if(InBounds(m,n))
				{
					avg += mHeightMap[m*mInfo.HeightmapWidth + n];
					num += 1.0f;
				}
			}
		}

		return avg / num;
	}

	//! Returns the width, in world coordinates.
	float Terrain::GetWidth()
	{
		// Total terrain width.
		return (mInfo.HeightmapWidth-1)*mInfo.CellSpacing;
	}

	//! Returns the depth, in world coordinates.
	float Terrain::GetDepth()
	{
		// Total terrain depth.
		return (mInfo.HeightmapHeight-1)*mInfo.CellSpacing;
	}

	//! Returns the height at (x, z).
	float Terrain::GetHeight(float x, float z)
	{
		// Transform from terrain local space to "cell" space.
		float c = (x + 0.5f*GetWidth()) /  mInfo.CellSpacing;
		float d = (z - 0.5f*GetDepth()) / -mInfo.CellSpacing;

		// Get the row and column we are in.
		int row = (int)floorf(d);
		int col = (int)floorf(c);

		if(!InBounds(row, col))
			return -numeric_limits<float>::infinity();

		// Grab the heights of the cell we are in.
		// A*--*B
		//  | /|
		//  |/ |
		// C*--*D
		float A = mHeightMap[row*mInfo.HeightmapWidth + col];
		float B = mHeightMap[row*mInfo.HeightmapWidth + col + 1];
		float C = mHeightMap[(row+1)*mInfo.HeightmapWidth + col];
		float D = mHeightMap[(row+1)*mInfo.HeightmapWidth + col + 1];

		// Where we are relative to the cell.
		float s = c - (float)col;
		float t = d - (float)row;

		// If upper triangle ABC.
		if( s + t <= 1.0f)
		{
			float uy = B - A;
			float vy = C - A;
			return A + s*uy + t*vy;
		}
		else // lower triangle DCB.
		{
			float uy = C - D;
			float vy = B - D;
			return D + (1.0f-s)*uy + (1.0f-t)*vy;
		}
	}

	//! Builds the heightmap SRV.
	void Terrain::BuildHeightmapSRV(ID3D11Device* device)
	{
		// Fill out the texture description.
		D3D11_TEXTURE2D_DESC texDesc;
		texDesc.Width = mInfo.HeightmapWidth;
		texDesc.Height = mInfo.HeightmapHeight;
		texDesc.MipLevels = 1;
		texDesc.ArraySize = 1;
		texDesc.Format    = DXGI_FORMAT_R16_FLOAT;
		texDesc.SampleDesc.Count   = 1;
		texDesc.SampleDesc.Quality = 0;
		texDesc.Usage = D3D11_USAGE_DEFAULT;
		texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		texDesc.CPUAccessFlags = 0;
		texDesc.MiscFlags = 0;

		/**
			The tricky part here is that we copy the values from the vector<float> heightmap into a ID3D11Texture2D.
		*/

		// HALF is defined in xnamath.h, for storing 16-bit float.
		std::vector<HALF> hmap(mHeightMap.size());
		std::transform(mHeightMap.begin(), mHeightMap.end(), hmap.begin(), XMConvertFloatToHalf);
	
		// Set the initial data to be the heightmap.
		D3D11_SUBRESOURCE_DATA data;
		data.pSysMem = &hmap[0];
		data.SysMemPitch = mInfo.HeightmapWidth*sizeof(HALF);
		data.SysMemSlicePitch = 0;

		// Create the texture.
		ID3D11Texture2D* hmapTex = 0;
		HR(device->CreateTexture2D(&texDesc, &data, &hmapTex));

		// Create the SRV to the texture.
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		srvDesc.Format = texDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = -1;
		HR(device->CreateShaderResourceView(hmapTex, &srvDesc, &mHeightMapSRV));

		// SRV saves reference.
		ReleaseCOM(hmapTex);
	}

	//! Builds the blendmap SRV.
	void Terrain::BuildBlendMapSRV(ID3D11Device* device)

	{
		// Fill out the texture description.
		D3D11_TEXTURE2D_DESC texDesc;
		texDesc.Width = mInfo.HeightmapWidth;
		texDesc.Height = mInfo.HeightmapHeight;
		texDesc.MipLevels = 1;
		texDesc.ArraySize = 1;
		texDesc.Format    = DXGI_FORMAT_R32G32B32A32_FLOAT;
		texDesc.SampleDesc.Count   = 1;
		texDesc.SampleDesc.Quality = 0;
		texDesc.Usage = D3D11_USAGE_DYNAMIC;
		texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		texDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		texDesc.MiscFlags = 0;

		// Set the initial data to be the heightmap.
		D3D11_SUBRESOURCE_DATA data;
		data.pSysMem = &mBlendMap[0];
		data.SysMemPitch = mInfo.HeightmapWidth*sizeof(XMFLOAT4);
		data.SysMemSlicePitch = 0;

		// Create the texture.
		ID3D11Texture2D* bmapTex = 0;
		HR(device->CreateTexture2D(&texDesc, &data, &bmapTex));

		// Create the SRV to the texture.
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		srvDesc.Format = texDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = -1;
		HR(device->CreateShaderResourceView(bmapTex, &srvDesc, &mBlendMapSRV));

		return;

		// Get the texture.
		ID3D11Resource* resource = nullptr;
		mBlendMapSRV->GetResource(&resource);

		ID3D11Texture2D* texture = nullptr;
		resource->QueryInterface(__uuidof(ID3D11Texture2D), (void**)&texture);

		// Map the texture.
		D3D11_MAPPED_SUBRESOURCE mappedData;
		GlobalApp::GetD3DContext()->Map(texture, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);

		const uint32_t pixelSize = sizeof(XMFLOAT4);	// 12
		const uint32_t srcPitch = pixelSize * mInfo.HeightmapWidth;
		uint8_t* textureData = reinterpret_cast<uint8_t*>(mappedData.pData);
		const uint8_t* srcData = reinterpret_cast<uint8_t*>(mBlendMap.data());
		for(uint32_t i = 0; i < mInfo.HeightmapHeight; ++i)
		{
			memcpy(textureData, srcData, srcPitch);

			textureData += mappedData.RowPitch;
			srcData += srcPitch;
		}

		GlobalApp::GetD3DContext()->Unmap(texture, 0);

		// SRV saves reference.
		ReleaseCOM(bmapTex);
	}

	//! Returns the InitInfo member.
	InitInfo Terrain::GetInfo()
	{
		return mInfo;
	}

	//! Returns the terrain primitive.
	Primitive* Terrain::GetPrimitive()
	{
		return mPrimitive;
	}

	//! Sets the height at (x, z) in the height map.
	void Terrain::SetHeigt(float x, float z, float height)
	{
		// Transform from terrain local space to "cell" space.
		float c = (x + 0.5f*GetWidth()) /  mInfo.CellSpacing;
		float d = (z - 0.5f*GetDepth()) / -mInfo.CellSpacing;

		// Get the row and column we are in.
		int row = (int)floorf(d);
		int col = (int)floorf(c);

		if(!InBounds(row, col))
			return;

		// Grab the heights of the cell we are in.
		// A*--*B
		//  | /|
		//  |/ |
		// C*--*D
		mHeightMap[row*mInfo.HeightmapWidth + col] = height;
	}

	//! Sets the blend value for layer at (pos.x, pos.z) in the blend map.
	void Terrain::SetBlend(XMFLOAT3 pos, float modifier, int layer)
	{
		// Transform from terrain local space to "cell" space.
		float c = (pos.x + 0.5f*GetWidth()) /  mInfo.CellSpacing;
		float d = (pos.z - 0.5f*GetDepth()) / -mInfo.CellSpacing;

		// Get the row and column we are in.
		int row = (int)floorf(d);
		int col = (int)floorf(c);

		if(!InBounds(row, col))
			return;

		// Subtract the modifier from all layers.
		XMFLOAT4& arrayReference = mBlendMap[row*mInfo.HeightmapWidth + col];
		XMStoreFloat4(&arrayReference, XMLoadFloat4(&arrayReference) - XMVectorSet(modifier, modifier, modifier, modifier));

		// Add the modififer to the active layer.
		if(layer == 0)
			mBlendMap[row*mInfo.HeightmapWidth + col].x += 2*modifier;
		else if(layer == 1)
			mBlendMap[row*mInfo.HeightmapWidth + col].y += 2*modifier;
		else if(layer == 2)
			mBlendMap[row*mInfo.HeightmapWidth + col].z += 2*modifier;
		else if(layer == 3)
			mBlendMap[row*mInfo.HeightmapWidth + col].w += 2*modifier;

		// Limit to 0-1 range.
		XMVECTOR limited = XMVectorMax(XMLoadFloat4(&arrayReference), XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f));
		XMStoreFloat4(&arrayReference, limited);
		limited = XMVectorMin(XMLoadFloat4(&arrayReference), XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f));
		XMStoreFloat4(&arrayReference, limited);
	}

	//! Returns the intersect point with the terrain.
	XMFLOAT3 Terrain::GetIntersectPoint(Ray ray)
	{
		ray.direction =  ray.direction * 600;
		Ray shorterRay = LinearSearch(ray);
		return BinarySearch(shorterRay);
	}

	//! Does a broad linear search.
	Ray Terrain::LinearSearch(Ray ray)
	{
		XMStoreFloat3(&ray.direction, XMLoadFloat3(&ray.direction) / 300.0f);
		XMFLOAT3 nextPoint = ray.origin + ray.direction;
		float heightAtNextPoint = GetHeight(nextPoint.x, nextPoint.z);
		int counter = 0;
		while(heightAtNextPoint < nextPoint.y && counter < 1000)
		{
			counter++;
			ray.origin = nextPoint;
			nextPoint = ray.origin + ray.direction;
			heightAtNextPoint = GetHeight(nextPoint.x, nextPoint.z);
		}

		// Return infinity if the ray dont strike anything.
		if(counter >= 1000) 
			ray.direction.x = numeric_limits<float>::infinity();

		return ray;
	}

	//! Does a binary search.
	XMFLOAT3 Terrain::BinarySearch(Ray ray)
	{
		float accuracy = 0.1f;
		float heightAtStartingPoint = GetHeight(ray.origin.x, ray.origin.z);
		float currentError = ray.origin.y - heightAtStartingPoint;
		int counter = 0;
		while(currentError > accuracy && counter < 1000)
		{
			counter++;
			// Divide by 2.
			XMStoreFloat3(&ray.direction, XMLoadFloat3(&ray.direction) / 2.0f);

			XMFLOAT3 nextPoint = ray.origin + ray.direction;
			float heightAtNextPoint = GetHeight(nextPoint.x, nextPoint.z);

			// Is the next point above the terrain?
			if(nextPoint.y > heightAtNextPoint)
			{
				ray.origin = nextPoint;
				currentError = ray.origin.y - heightAtNextPoint;
			}
		}

		// Return infinity if the ray dont strike anything.
		if(counter >= 1000) 
			return XMFLOAT3(numeric_limits<float>::infinity(), 0, 0);
		
		return ray.origin;
	}
}	// End of Graphics Library namespace.