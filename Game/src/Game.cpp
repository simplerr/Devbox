#include <crtdbg.h> 
//#include "vld.h"
#include <assert.h>
#include <time.h>
#include "Game.h"
#include "Graphics.h"
#include "Input.h"
#include "World.h"
#include "Primitive.h"
#include "Camera.h"
#include "Effects.h"
#include "CameraFPS.h"
#include "D3DCore.h"

using namespace GLib;

// Linking to Rendering.lib
#ifdef _DEBUG
#pragma comment(lib, "../debug/Rendering.lib")
#else
#pragma comment(lib, "../release/Rendering.lib")
#endif

//! The program starts here.
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
	// Create a Game instance.
	Game game(hInstance, "Devbox testbed", 1400, 1000);

	GlobalApp::SetRunnable(&game);

	// Init the app.
	auto i = GlobalApp::GetGame();
	GlobalApp::GetGame()->Init();

	//GlobalApp::GetGame()->ResizeWindow(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
	
	auto core = GlobalApp::GetWindowHandler();

	// Run the app.
	return GlobalApp::GetGame()->Run();
}

Game::Game(HINSTANCE hInstance, string caption, int width, int height)
	: Runnable(hInstance, caption, width, height)
{
	// Cap the fps to 100.
	//SetFpsCap(99.0f);
}
	
Game::~Game()
{

}

void Game::Init()
{
	// Important to run Systems Init() function.
	Runnable::Init();

	// Add a camera.
	GLib::CameraFPS* camera = new GLib::CameraFPS();
	GetGraphics()->SetCamera(camera);

	// Set the fog color.
	GetGraphics()->SetFogColor(XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f));

	mDrawDebug = false;

	mTexture = GetGraphics()->LoadTexture("swag.bmp");
	mPosition = XMFLOAT2(800, 800);
}

void Game::Update(GLib::Input* pInput, float dt)
{
	GetGraphics()->Update(pInput, dt);

	if(pInput->KeyDown('A'))
		mPosition.x -= 1.0f;
	else if(pInput->KeyDown('D'))
		mPosition.x += 1.0f;

	if(pInput->KeyPressed(VK_SPACE))
	{
		char buffer[244];
		sprintf(buffer, "x: %.2f\ny: %.2f\nFPS:%.2f", 0.0f, 0.0f, GetCurrentFps());
		OutputDebugString(buffer);
	}

	if(pInput->KeyPressed(VK_F1))
		mDrawDebug = !mDrawDebug;

	/*if(pInput->KeyReleased('F')) {
		float width = GetSystemMetrics(SM_CXSCREEN);
		float height = GetSystemMetrics(SM_CYSCREEN);

		ResizeWindow(width, height);
	}*/
}
	
void Game::Draw(GLib::Graphics* pGraphics)
{
	// Clear the render target and depth/stencil.
	pGraphics->ClearScene();

	//pGraphics->DrawBillboards();

	pGraphics->DrawScreenQuad(mTexture, mPosition.x, 800, 400, 400);

	if(mDrawDebug)
	{
		char buffer[244];
		sprintf(buffer, "x: %.2f\ny: %.2f\nFPS:%.2f", 0.0f, 0.0f, GetCurrentFps());
		pGraphics->DrawText(buffer, GLib::GlobalApp::GetClientWidth()-100, 400, 20, GLib::ColorRGBA(255, 255, 255, 255));
	}

	// Present the backbuffer.
	pGraphics->Present();

	// Unbind the SRVs from the pipeline so they can be used as DSVs instead.
	//ID3D11ShaderResourceView *const nullSRV[4] = {NULL, NULL, NULL, NULL};
	//pGraphics->GetContext()->PSSetShaderResources(0, 4, nullSRV);
	//Effects::GetBasicFX()->Apply(GLib::GlobalApp::GetD3DContext());
}

//! Called when the window gets resized.
void Game::OnResize(int width, int height)
{

}

LRESULT Game::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return Runnable::MsgProc(hwnd, msg, wParam, lParam);
}