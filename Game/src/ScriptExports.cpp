#include "ScriptExports.h"
#include "GlibStd.h"
#include "Input.h"
#include "LuaManager.h"
#include "Graphics.h"
#include "Camera.h"

void ScriptExports::Register()
{
	LuaPlus::LuaObject globals = LuaManager::Get()->GetGlobalVars();

	globals.RegisterDirect("key_pressed", &ScriptExports::KeyPressed);
	globals.RegisterDirect("key_down", &ScriptExports::KeyDown);
	globals.RegisterDirect("get_cursor_pos", &ScriptExports::GetCursorPosition);

	globals.RegisterDirect("draw_string", &ScriptExports::DrawString);
	globals.RegisterDirect("draw_texture", &ScriptExports::DrawTexture);
	globals.RegisterDirect("draw_aabb", &ScriptExports::DrawAABB);

	globals.RegisterDirect("get_camera_pos", &ScriptExports::GetCameraPosition);
	globals.RegisterDirect("get_camera_dir", &ScriptExports::GetCameraDirection);
	globals.RegisterDirect("get_camera_target", &ScriptExports::GetCameraTarget);
	globals.RegisterDirect("set_camera_pos", &ScriptExports::SetCameraPosition);
	globals.RegisterDirect("set_camera_dir", &ScriptExports::SetCameraDirection);
	globals.RegisterDirect("set_camera_target", &ScriptExports::SetCameraTarget);


}

bool ScriptExports::KeyPressed(const char* key)
{
	std::string str = key;

	if(GLib::GlobalApp::GetInput()->KeyPressed(str.size() != 1 ? MapToKey(str) : key[0]))
		return true;
	else
		return false;
}

bool ScriptExports::KeyDown(const char* key)
{
	std::string str = key;

	if(GLib::GlobalApp::GetInput()->KeyDown(str.size() != 1 ? MapToKey(str) : key[0]))
		return true;
	else
		return false;
}

LuaPlus::LuaObject ScriptExports::GetCursorPosition()
{
	XMFLOAT3 pos = GLib::GlobalApp::GetInput()->MousePosition();

	LuaPlus::LuaObject luaPos;
	LuaManager::Get()->ConvertVec3ToTable(pos, luaPos);

	return luaPos;
}

void ScriptExports::DrawString(const char* text, int x, int y, int size)
{
	GLib::GlobalApp::GetGraphics()->DrawText(text, x, y, size);
}

void ScriptExports::DrawTexture(const char* texture, int x, int y, int width, int height)
{
	GLib::GlobalApp::GetGraphics()->DrawScreenTexture(texture, x, y, width, height);
}

void ScriptExports::DrawAABB(float x, float y, float z, float extents, float alpha)
{
	XNA::AxisAlignedBox box;
	box.Center = XMFLOAT3(x, y, z);
	box.Extents = XMFLOAT3(extents, extents, extents);
	auto material = GLib::Material(GLib::Colors::Green);
	GLib::GlobalApp::GetGraphics()->DrawBoundingBox(&box, XMMatrixIdentity(), material, alpha);
}

LuaPlus::LuaObject ScriptExports::GetCameraPosition()
{
	XMFLOAT3 pos = GLib::GlobalApp::GetGraphics()->GetCamera()->GetPosition();
	LuaPlus::LuaObject posTable;
	LuaManager::Get()->ConvertVec3ToTable(pos, posTable);

	return posTable;
}

LuaPlus::LuaObject ScriptExports::GetCameraDirection()
{
	XMFLOAT3 dir = GLib::GlobalApp::GetGraphics()->GetCamera()->GetDirection();
	LuaPlus::LuaObject dirTable;
	LuaManager::Get()->ConvertVec3ToTable(dir, dirTable);

	return dirTable;
}

LuaPlus::LuaObject ScriptExports::GetCameraTarget()
{
	XMFLOAT3 target = GLib::GlobalApp::GetGraphics()->GetCamera()->GetTarget();
	LuaPlus::LuaObject targetTable;
	LuaManager::Get()->ConvertVec3ToTable(target, targetTable);

	return targetTable;
}

void ScriptExports::SetCameraPosition(float x, float y, float z)
{
	GLib::GlobalApp::GetGraphics()->GetCamera()->SetPosition(XMFLOAT3(x, y, z));
}

void ScriptExports::SetCameraDirection(float x, float y, float z)
{
	GLib::GlobalApp::GetGraphics()->GetCamera()->SetDirection(XMFLOAT3(x, y, z));
}

void ScriptExports::SetCameraTarget(float x, float y, float z)
{
	GLib::GlobalApp::GetGraphics()->GetCamera()->SetTarget(XMFLOAT3(x, y, z));
}


int ScriptExports::MapToKey(std::string str)
{
	if(str == "VK_LBUTTON")
		return VK_LBUTTON;
	else if(str == "VK_RBUTTON")
		return VK_RBUTTON;
	else if(str == "VK_MBUTTON")
		return VK_MBUTTON;
	else if(str == "VK_SPACE")
		return VK_SPACE;
	else if(str == "VK_ESCAPE")
		return VK_ESCAPE;
	else if(str == "VK_DELETE")
		return VK_DELETE;
	else if(str == "VK_CONTROL")
		return VK_CONTROL;

	return 0;
}