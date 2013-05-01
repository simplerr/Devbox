#include "D3DCore.h"
#include "Utility.h"
#include "Runnable.h"

//! Graphics Library namespace.
namespace GLib
{

//! Constructor. The Init() function handles the initialization.
D3DCore::D3DCore()
	: 	
	md3dDevice(0),
	md3dImmediateContext(0),
	mSwapChain(0),
	mDepthStencilBuffer(0),
	mRenderTargetView(0),
	mDepthStencilView(0)
{
	m4xMsaaQuality = 0;
	mEnable4xMsaa = true;
	mDimensionRatios.right = 1.0f;
	mDimensionRatios.bottom = 1.0f;
}

//! Cleans up and frees all pointers.
D3DCore::~D3DCore()
{

}

//! Initializes Direct3D. Code taken and modified from Frank D Lunas book "Introduction to 3d game programming with DirectX 11".
/**
@return false if something went wrong.
*/
bool D3DCore::Init(int clientWidth, int clientHeight, HWND hwnd, bool fullscreen)
{
	// Set the client width and height. Will be used again in OnResize().
	mClientWidth = clientWidth;
	mClientHeight = clientHeight;

	// Create the device and device context.
	UINT createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)  
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL featureLevel;
	HRESULT hr = D3D11CreateDevice(
			0,                 // default adapter
			D3D_DRIVER_TYPE_HARDWARE,
			0,                 // no software device
			createDeviceFlags, 
			0, 0,              // default feature level array
			D3D11_SDK_VERSION,
			&md3dDevice,
			&featureLevel,
			&md3dImmediateContext);

	if(FAILED(hr)) {
		MessageBox(0, "D3D11CreateDevice Failed.", 0, 0);
		return false;
	}

	if(featureLevel != D3D_FEATURE_LEVEL_11_0) {
		MessageBox(0, "Direct3D Feature Level 11 unsupported.", 0, 0);
		return false;
	}

	// Check 4X MSAA quality support for our back buffer format.
	// All Direct3D 11 capable devices support 4X MSAA for all render 
	// target formats, so we only need to check quality support.
	HR(md3dDevice->CheckMultisampleQualityLevels(
		DXGI_FORMAT_R8G8B8A8_UNORM, 4, &m4xMsaaQuality));
	assert(m4xMsaaQuality > 0);

	// Fill out a DXGI_SWAP_CHAIN_DESC to describe our swap chain.
	DXGI_SWAP_CHAIN_DESC sd;
	sd.BufferDesc.Width  = mClientWidth;
	sd.BufferDesc.Height = mClientHeight;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// Use 4X MSAA? 
	if(mEnable4xMsaa) {
		sd.SampleDesc.Count   = 4;
		sd.SampleDesc.Quality = m4xMsaaQuality-1;
	}
	// No MSAA
	else {
		sd.SampleDesc.Count   = 1;
		sd.SampleDesc.Quality = 0;
	}

	sd.BufferUsage  = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount  = 1;
	sd.OutputWindow = hwnd;
	sd.Windowed     = !fullscreen;
	sd.SwapEffect   = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags        = 0;//DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	// To correctly create the swap chain, we must use the IDXGIFactory that was
	// used to create the device.  If we tried to use a different IDXGIFactory instance
	// (by calling CreateDXGIFactory), we get an error: "IDXGIFactory::CreateSwapChain: 
	// This function is being called with a device from a different IDXGIFactory."
	IDXGIDevice* dxgiDevice = 0;
	HR(md3dDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice));
	      
	IDXGIAdapter* dxgiAdapter = 0;
	HR(dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter));

	IDXGIFactory* dxgiFactory = 0;
	HR(dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory));

	HR(dxgiFactory->CreateSwapChain(md3dDevice, &sd, &mSwapChain));
	
	ReleaseCOM(dxgiDevice);
	ReleaseCOM(dxgiAdapter);
	ReleaseCOM(dxgiFactory);

	// The remaining steps that need to be carried out for d3d creation
	// also need to be executed every time the window is resized.  So
	// just call the OnResize method here to avoid code duplication.
	OnResize(mClientWidth, mClientHeight);

	return true;
}

//! Called when the window gets resized. Code taken and modified from Frank D Lunas book "Introduction to 3d game programming with DirectX 11".
/*

*/
void D3DCore::OnResize(int width, int height)
{
	assert(md3dImmediateContext);
	assert(md3dDevice);
	assert(mSwapChain);

	// Release the old views, as they hold references to the buffers we
	// will be destroying.  Also release the old depth/stencil buffer.
	ReleaseCOM(mRenderTargetView);
	ReleaseCOM(mDepthStencilView);
	ReleaseCOM(mDepthStencilBuffer);

	// Resize the swap chain and recreate the render target view.
	HR(mSwapChain->ResizeBuffers(1, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0));
	ID3D11Texture2D* backBuffer;
	HR(mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer)));
	HR(md3dDevice->CreateRenderTargetView(backBuffer, 0, &mRenderTargetView));
	ReleaseCOM(backBuffer);

	// Create the depth/stencil buffer and view.
	D3D11_TEXTURE2D_DESC depthStencilDesc;
	
	depthStencilDesc.Width     = width;
	depthStencilDesc.Height    = height;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format    = DXGI_FORMAT_D24_UNORM_S8_UINT;

	// Use 4X MSAA? --must match swap chain MSAA values.
	if( mEnable4xMsaa ) {
		depthStencilDesc.SampleDesc.Count   = 4;
		depthStencilDesc.SampleDesc.Quality = m4xMsaaQuality-1;
	}
	// No MSAA
	else {
		depthStencilDesc.SampleDesc.Count   = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
	}

	depthStencilDesc.Usage          = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags      = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0; 
	depthStencilDesc.MiscFlags      = 0;

	HR(md3dDevice->CreateTexture2D(&depthStencilDesc, 0, &mDepthStencilBuffer));
	HR(md3dDevice->CreateDepthStencilView(mDepthStencilBuffer, 0, &mDepthStencilView));

	// Bind the render target view and depth/stencil view to the pipeline.
	md3dImmediateContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);

	// Set the viewport transform.
	mScreenViewport.TopLeftX = 0;
	mScreenViewport.TopLeftY = 0;
	mScreenViewport.Width    = static_cast<float>(width);
	mScreenViewport.Height   = static_cast<float>(height);
	mScreenViewport.MinDepth = 0.0f;
	mScreenViewport.MaxDepth = 1.0f;

	md3dImmediateContext->RSSetViewports(1, &mScreenViewport);

	// [HACK]
	mDimensionRatios.right = (float)1400 / width;
	mDimensionRatios.bottom = (float)1000 / height;
}

//! Calls the swapchain Present() function that swaps the front- and backbuffer.
HRESULT D3DCore::Present(UINT SyncInterval, UINT Flags)
{
	return mSwapChain->Present(SyncInterval, Flags);
}

//! Sets the fullscreen state.
void D3DCore::SetFullScreen(int width, int height, bool fullscreen)
{
	mSwapChain->SetFullscreenState(fullscreen, NULL);
	OnResize(width, height);	// [NOTE] Ugly.

	if(fullscreen) {
		mDimensionRatios.right = (float)width / mClientWidth;
		mDimensionRatios.bottom = (float)height / mClientHeight;
	}
	else {
		mDimensionRatios.right = 1.0f;
		mDimensionRatios.bottom = 1.0f;
	}
}

//! Returns the ID3D11Device*.
ID3D11Device* D3DCore::GetDevice()
{
	return md3dDevice;
}

//! Returns the ID3D11DeviceContext*.
ID3D11DeviceContext* D3DCore::GetContext()
{
	return md3dImmediateContext;
}

//! Returns the IDXGISwapChain*.
IDXGISwapChain*	D3DCore::GetSwapChain()
{
	return mSwapChain;
}
	
//! Returns the ID3D11RenderTargetView*.
ID3D11RenderTargetView* D3DCore::GetRenderTargetView()
{
	return mRenderTargetView;
}
	
//! Returns the ID3D11DepthStencilView*.
ID3D11DepthStencilView* D3DCore::GetDepthStencilView()
{
	return mDepthStencilView;
}

D3D11_VIEWPORT	D3DCore::GetViewport()
{
	return mScreenViewport;
}
// [NOTE] Runnable has these as well.
float D3DCore::GetClientWidth()
{
	return mClientWidth;
}
	
float D3DCore::GetClientHeight()
{
	return mClientHeight;
}

Rect D3DCore::GetDimensionRatio()
{
	return mDimensionRatios;
}

}	// End of Graphics Library namespace.
