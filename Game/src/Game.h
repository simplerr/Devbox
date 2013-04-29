#pragma once
#include "Runnable.h"
#include "Utility.h"

// Forward declarations.
#pragma region Forward declarations.
namespace GLib {
	class Light;
	class RenderTarget;
	class Primitive;
	class ShadowMap;
	class ModelImporter;
	class Model;
	class SkinnedModel;
	class World;
	class StaticObject;
	class AnimatedObject;
	struct Texture2D;
}

class ControlManager;
class GameState;
class ActorManager;
class ActorFactory;
class ChunkManager;

#pragma endregion

class Game : public GLib::Runnable
{
public:
	Game(HINSTANCE hInstance, string caption, int width, int height);
	~Game();

	void Init();
	void Update(GLib::Input* pInput, float dt);
	void Draw(GLib::Graphics* pGraphics);
	void OnResize(int width, int height);
	void ChangeState(GameState* pGameState);

	LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	void InitWorld();
	void ExecuteLuaScripts();
	void ReloadActors();

	void AddActor(const char* name);
private:
	bool mDrawDebug;
	GLib::World* mWorld;
	GLib::Texture2D* texture;
	ActorManager* mActorManager;
	ActorFactory* mActorFactory;
	ChunkManager* mChunkManager;
};