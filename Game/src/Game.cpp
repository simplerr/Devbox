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
#include "Terrain.h"
#include "StaticObject.h"
#include "ShadowMap.h"
#include "GlibStd.h"
#include "ActorManager.h"
#include "Actor.h"
#include "TransformComponent.h"
#include "BoundingBoxComponent.h"
#include "ActorFactory.h"
#include "LuaManager.h"
#include "DebugConsole.h"
#include "ScriptComponent.h"

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

	Logger::Init("log.txt");
	DebugConsole::Init(20, 20, 700, 320);

	if (!LuaManager::Create())
		GLIB_ERROR("Failed to initialize Lua");

	// Init the app.
	GlobalApp::GetGame()->Init();

	// Run the app.
	return GlobalApp::GetGame()->Run();
}

Game::Game(HINSTANCE hInstance, string caption, int width, int height)
	: Runnable(hInstance, caption, width, height)
{
	// Cap the fps to 100.
	//SetFpsCap(99.0f);
	mDrawDebug = false;
}
	
Game::~Game()
{
	Logger::Destroy();
}

void Game::Init()
{
	// Important to run Systems Init() function.
	Runnable::Init();

	GlobalApp::GetGame()->SetWindowPosition(20, 350);

	InitWorld();

	// Testing actor stuff.
	mActorManager = new ActorManager;
	mActorFactory = new ActorFactory;

	ReloadActors();
}

void Game::ExecuteLuaScripts()
{
	// Execute all the different lua files of interest here.
	LuaManager::Get()->ExecuteFile("data/lua/default-actor.lua");
}

void Game::ReloadActors()
{
	// Clear the actor manager.
	mActorManager->Clear();

	ExecuteLuaScripts();
	//ScriptComponent::RegisterScriptFunctions();

	auto actor = mActorFactory->CreateActor("default_actor");
	auto actor2 = mActorFactory->CreateActor("default_actor");

	auto transform = MakeStrongPtr(actor2->GetComponent<TransformComponent>(TransformComponent::g_Name));
	transform->SetPosition(XMFLOAT3(-40, 30, 0));

	mActorManager->AddActor(actor);
	mActorManager->AddActor(actor2);
}

void Game::InitWorld()
{
	// Add a camera.
	GLib::CameraFPS* camera = new GLib::CameraFPS();
	camera->SetMovementSpeed(0.2f);
	camera->SetRotateButton(VK_MBUTTON);
	GetGraphics()->SetCamera(camera);

	// Set the fog color.
	GetGraphics()->SetFogColor(XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f));

	mWorld = new World();
	mWorld->Init(GetGraphics());

	// Connect the graphics light list.
	GLib::GlobalApp::GetGraphics()->SetLightList(mWorld->GetLights());
}

void Game::Update(GLib::Input* pInput, float dt)
{
	GetGraphics()->Update(pInput, dt);

	if(pInput->KeyPressed('R'))
		ReloadActors();

	mWorld->Update(dt);
	mActorManager->Update(dt);

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

	mWorld->Draw(pGraphics);
	mActorManager->Draw(pGraphics);

	pGraphics->DrawBillboards();

	if(mDrawDebug)
	{
		char buffer[244];
		sprintf(buffer, "x: %.2f\ny: %.2f\nFPS:%.2f", 0.0f, 0.0f, GetCurrentFps());
		pGraphics->DrawText(buffer, GLib::GlobalApp::GetClientWidth()-100, 400, 20, GLib::ColorRGBA(255, 255, 255, 255));
	}

	// Present the backbuffer.
	pGraphics->Present();

	// Unbind the SRVs from the pipeline so they can be used as DSVs instead.
	ID3D11ShaderResourceView *const nullSRV[4] = {NULL, NULL, NULL, NULL};
	pGraphics->GetContext()->PSSetShaderResources(0, 4, nullSRV);
	Effects::GetBasicFX()->Apply(GLib::GlobalApp::GetD3DContext());
}

//! Called when the window gets resized.
void Game::OnResize(int width, int height)
{

}

LRESULT Game::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return Runnable::MsgProc(hwnd, msg, wParam, lParam);
}