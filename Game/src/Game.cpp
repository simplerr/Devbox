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
#include "CameraDefault.h"
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
#include "ScriptExports.h"
#include "ChunkManager.h"
#include "LightObject.h"

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
	SetFpsCap(300.0f);
	mDrawDebug = false;
}
	
Game::~Game()
{
	Logger::Destroy();
	delete mChunkManager;
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

	texture = GlobalApp::GetGraphics()->LoadTexture("swag.bmp");
}

void Game::Update(GLib::Input* pInput, float dt)
{
	if(pInput->KeyPressed('R'))
		ReloadActors();

	mWorld->Update(dt);
	mActorManager->Update(dt);
	mChunkManager->Update(dt);

	GetGraphics()->Update(pInput, dt);

	if(pInput->KeyPressed(VK_F1))
		mDrawDebug = !mDrawDebug;
}
	
void Game::Draw(GLib::Graphics* pGraphics)
{
	// Clear the render target and depth/stencil.
	pGraphics->ClearScene();

	mWorld->Draw(pGraphics);
	mActorManager->Draw(pGraphics);
	mChunkManager->Draw(pGraphics);

	//pGraphics->DrawBillboards();

	pGraphics->DrawScreenTexture("swag.bmp", 700, 500, 10, 10);

	if(mDrawDebug)
	{
		char buffer[244];
		XMFLOAT3 cameraPos = pGraphics->GetCamera()->GetPosition();
		sprintf(buffer, "camera.x: %.2f\ncamera.y: %.2f\ncamera.y: %.2f\nx: %.2f\ny: %.2f\nFPS:%.2f", cameraPos.x, cameraPos.y, cameraPos.z, GLib::GlobalApp::GetInput()->MousePosition().x, GLib::GlobalApp::GetInput()->MousePosition().y, GetCurrentFps());
		pGraphics->DrawText(buffer, GLib::GlobalApp::GetClientWidth()-200, 400, 20, GLib::ColorRGBA(255, 255, 255, 255));
	}

	pGraphics->DrawBoundingBox(XMFLOAT3(0, 70, 0), 6, 6, 6, Colors::Red, false, 1.0f);

	// Present the backbuffer.
	pGraphics->Present();

	// Unbind the SRVs from the pipeline so they can be used as DSVs instead.
	ID3D11ShaderResourceView *const nullSRV[4] = {NULL, NULL, NULL, NULL};
	pGraphics->GetContext()->PSSetShaderResources(0, 4, nullSRV);
	Effects::GetBasicFX()->Apply(GLib::GlobalApp::GetD3DContext());
}

void Game::ExecuteLuaScripts()
{
	// Execute all the different lua files of interest here.
	LuaManager::Get()->ExecuteFile("data/lua/player.lua");
	LuaManager::Get()->ExecuteFile("data/lua/default-actor.lua");
	LuaManager::Get()->ExecuteFile("data/lua/default-scene.lua");

	// [TEMP] Move this to proper location
	// Register the add_actor function
	LuaPlus::LuaObject globals = LuaManager::Get()->GetGlobalVars();
	globals.RegisterDirect("add_actor", *this, &Game::AddActor);
}

void Game::ReloadActors()
{
	// Clear the actor manager.
	mActorManager->Clear();

	ExecuteLuaScripts();

	ScriptExports::Register();

	// Register the ChunkManager script functions.
	LuaPlus::LuaObject globals = LuaManager::Get()->GetGlobalVars();
	globals.RegisterDirect("get_block_height", *mChunkManager, &ChunkManager::LuaGetHeight);


	// [TEMP] Move this to proper location
	// Call "scene" on_created()
	LuaPlus::LuaObject object = LuaManager::Get()->GetGlobalVars()["on_created"];
	if(object.IsFunction())
	{
		LuaPlus::LuaFunction<void> on_created = object;
		on_created();
	}
}

void Game::AddActor(const char* name)
{
	StrongActorPtr actor = mActorFactory->CreateActor(name);

	if(actor != nullptr)
		mActorManager->AddActor(actor);
	else
	{
		GLIB_INFO(string("Actor \"") + name + "\" not found");
	}
}

void Game::InitWorld()
{
	// Set the fog color.
	GetGraphics()->SetFogColor(XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f));

	// Init the world that contains all the models.
	mWorld = new World();
	mWorld->Init(GetGraphics(), false);
	mWorld->SetWorldCenter(XMFLOAT3(10000, 0, 10000));	// [NOTE] (10000, 10000) is the center of the world!!

	// Add a camera.
	GLib::CameraDefault* camera = new GLib::CameraDefault();
	camera->SetMovementSpeed(0.02f);
	camera->SetPosition(mWorld->GetWorldCenter() + XMFLOAT3(0, 200, 0));
	GetGraphics()->SetCamera(camera);

	// Create the main light.
	GLib::LightObject* light = new GLib::LightObject();
	light->SetPosition(mWorld->GetWorldCenter() + XMFLOAT3(20, 50, 20));
	light->SetRotation(XMFLOAT3(1, -0.5, 0));
	light->SetLightType(DIRECTIONAL_LIGHT);
	light->SetMaterials(GLib::Material(GLib::Colors::White));

	mWorld->AddObject(light);

	mChunkManager = new ChunkManager;

	// Connect the graphics light list.
	GLib::GlobalApp::GetGraphics()->SetLightList(mWorld->GetLights());
}

//! Called when the window gets resized.
void Game::OnResize(int width, int height)
{

}

LRESULT Game::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return Runnable::MsgProc(hwnd, msg, wParam, lParam);
}