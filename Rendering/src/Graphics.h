#pragma once

#include <Windows.h>
#include <string>
#include <xnamath.h>
#include <vector>
#include <map>
#include "Light.h"
#include "Utility.h"
#include "xnacollision.h"
#include "FW1FontWrapper.h"

using namespace std;
using namespace XNA;

//! Graphics Library namespace.
namespace GLib
{
	// Forward declarations.
	class D3DCore;
	class BasicEffect;
	class Primitive;
	class EffectManager;
	class BillboardManager;
	class RenderTarget;
	class Camera;
	class Sky;
	class BlurFilter;
	class ShadowMap;
	class SkinnedModel;
	class StaticModel;
	class PrimitiveFactory;
	class ModelImporter;
	class Input;
	struct Material;
	struct Texture2D;
	struct BillboardVertex;
	class Light;

	//! Represents a 2D texture.
	struct Texture2D
	{
		Texture2D() {
			shaderResourceView = 0;
			scale = 1.0f;
			name = "none";
		}

		ID3D11ShaderResourceView* shaderResourceView;
		float scale;
		string name;
	};

	//!
	//	The user interface to the graphical components.
	//	Contains a D3DCore instance.
	//	This class and it's members handles everything that has to do with Direct3D.
	//!
	class GLIB_API Graphics
	{
	public:
		Graphics();
		~Graphics();

		bool Init(int clientWidth, int clientHeight, HWND hwnd, bool fullscreen = false);
		void Update(Input* pInput, float dt);

		void DrawPrimitive(Primitive* primitive, CXMMATRIX worldMatrix, Texture2D* texture, Texture2D* normalMap, Material material, BasicEffect* effect);
		void DrawBillboards();
		void DrawScreenTexture(Texture2D* texture, float x, float y, float width, float height);
		void DrawScreenTexture(string texture, float x, float y, float width, float height);
		void DrawBoundingBox(AxisAlignedBox* aabb, CXMMATRIX worldMatrix, Material material, float transparency = 0.4f);
		void DrawText(string text, int x, int y, int size,  UINT32 color = 0xff000000, string fontFamily = "Arial");
		void ActiveShadowMap();
		void DeactiveShadowMap();
		void ApplyBlur(Texture2D* texture, int blurCount);
		Rect MeasureText(string text, int size, string fontFamily);

		Texture2D* LoadTexture(string filename, float scale = 1.0f);
		BillboardVertex* AddBillboard(XMFLOAT3 position, XMFLOAT2 size, string texture);
		void RemoveBillboard(string texture, BillboardVertex* pBillboard);

		void ClearScene();
		void Present();

		void SetEffectParameters(BasicEffect* effect, CXMMATRIX worldMatrix, Texture2D* texture, Texture2D* normalMap, Material material);
		void SetLightList(LightList* lightList);
		void SetFogColor(XMFLOAT4 color);
		void SetRenderTarget(RenderTarget* renderTarget);
		void SetCamera(Camera* pCamera);
		void RestoreRenderTarget();
		XMFLOAT2 TransformToScreenSpace(XMFLOAT3 position);

		// Getters.
		ID3D11DeviceContext*	GetContext();
		ID3D11Device*			GetDevice();
		D3DCore*				GetD3D();
		Camera*					GetCamera();
		ShadowMap*				GetShadowMap();
		LightList*				GetLightList();
		PrimitiveFactory*		GetPrimitiveFactory();
		ModelImporter*			GetModelImporter();
		XMFLOAT4				GetFogColor();
		float					GetClientWidth();
		float					GetClientHeight();
		bool					IsRenderingShadows();
	private:
		map<string, Texture2D*>			mTextureMap;
		map<string, BillboardManager*>	mBillboardManagerMap;
		LightList*		mLightList;
		D3DCore*		mD3DCore;
		Camera*			mCamera;
		BlurFilter*		mBlurFilter;
		Primitive*		mScreenQuad;
		Primitive*		mAABB;
		ShadowMap*		mShadowMap;
		PrimitiveFactory* mPrimitiveFactory;
		ModelImporter*	mModelImporter;
		XMFLOAT4		mFogColor;
		bool			mRenderingShadows;

		// Font.
		IFW1Factory*	 mFontFactory;
		IFW1FontWrapper* mFontWrapper;
 	};	// Class
}