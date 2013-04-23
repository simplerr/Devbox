#include "ScriptComponent.h"
#include "Light.h"
#include "LuaManager.h"
#include "GlibStd.h"
#include "Actor.h"
#include "Input.h"
#include "TransformComponent.h"
#include "StaticModelComponent.h"
#include "AnimatedModelComponent.h"


// This is the name of the metatable where all the function definitions exported to Lua will live.
static const char* METATABLE_NAME = "ScriptComponentMetaTable";
const char* ScriptComponent::g_Name = "ScriptComponent";

ScriptComponent::ScriptComponent()
{
	mLuaObject.AssignNil(LuaManager::Get()->GetLuaState());
}

void ScriptComponent::Init(LuaPlus::LuaObject initData)
{
	
}

void ScriptComponent::PostInit()
{
	// Register the C++ functions to the metatable.
	RegisterScriptFunctions();

	// Setup the Lua export data.
	mLuaObject.AssignNewTable(LuaManager::Get()->GetLuaState());
	CreateScriptObject();

	mLuaOnCreated = mLuaObject["on_created"];
	mLuaOnUpdate = mLuaObject["on_update"];
	mLuaOnDraw = mLuaObject["on_draw"];

	if(mLuaOnCreated.IsFunction())
	{
		LuaPlus::LuaFunction<void> on_created(mLuaOnCreated);
		on_created(mLuaObject);
	}
}

void ScriptComponent::Update(float dt)
{
	if(mLuaOnUpdate.IsFunction())
	{
		LuaPlus::LuaFunction<void> on_update(mLuaOnUpdate);
		on_update(mLuaObject, dt);
	}
}

void ScriptComponent::Draw(GLib::Graphics* pGraphics)
{
	if(mLuaOnDraw.IsFunction())
	{
		LuaPlus::LuaFunction<void> on_draw(mLuaOnDraw);
		on_draw(mLuaObject);
	}
}

void ScriptComponent::CreateScriptObject()
{
	LuaManager* luaManager = LuaManager::Get();

	GLIB_ASSERT(luaManager);
	GLIB_ASSERT(!mLuaObject.IsNil());

	string actorName = mOwner->GetName();
	LuaPlus::LuaObject metaTable = luaManager->GetGlobalVars().Lookup(actorName.c_str());
	GLIB_ASSERT(!metaTable.IsNil());

	LuaPlus::LuaObject boxedPtr = luaManager->GetLuaState()->BoxPointer(this);
	boxedPtr.SetMetaTable(metaTable);
	mLuaObject.SetLightUserData("__object", this);
	mLuaObject.SetMetaTable(metaTable);
}

void ScriptComponent::RegisterScriptFunctions()
{
	// Expand to the metatable,
	// the metatable already exists so we grab it 
	// and registers the C++ functions on it.
	string actorName = mOwner->GetName();
	LuaPlus::LuaObject metaTableObj = LuaManager::Get()->GetGlobalVars()[actorName.c_str()];
	
	// Is the metatable already registered?
	if(!metaTableObj["__index"].IsNil())
		return;

	metaTableObj.SetObject("__index", metaTableObj);

	metaTableObj.RegisterObjectDirect("GetActorId",		        (ScriptComponent*)0, &ScriptComponent::GetActorId);

	// Transform
	metaTableObj.RegisterObjectDirect("SetPosition",	        (ScriptComponent*)0, &ScriptComponent::SetPosition);
	metaTableObj.RegisterObjectDirect("SetRotation",	        (ScriptComponent*)0, &ScriptComponent::SetRotation);
	metaTableObj.RegisterObjectDirect("SetScale",				(ScriptComponent*)0, &ScriptComponent::SetScale);
	metaTableObj.RegisterObjectDirect("GetPosition",	        (ScriptComponent*)0, &ScriptComponent::GetPosition);
	metaTableObj.RegisterObjectDirect("GetRotation",	        (ScriptComponent*)0, &ScriptComponent::GetRotation);
	metaTableObj.RegisterObjectDirect("GetScale",				(ScriptComponent*)0, &ScriptComponent::GetScale);

	// Model
	metaTableObj.RegisterObjectDirect("SetAlpha",						(ScriptComponent*)0, &ScriptComponent::SetAlpha);
	metaTableObj.RegisterObjectDirect("SetAnimationIndex",				(ScriptComponent*)0, &ScriptComponent::SetAnimationIndex);
	metaTableObj.RegisterObjectDirect("SetAnimationSpeed",				(ScriptComponent*)0, &ScriptComponent::SetAnimationSpeed);
	metaTableObj.RegisterObjectDirect("GetAlpha",						(ScriptComponent*)0, &ScriptComponent::GetAlpha);
}

void ScriptComponent::SetPosition(float x, float y, float z)
{
	auto transform = MakeStrongPtr(mOwner->GetComponent<TransformComponent>(TransformComponent::g_Name));

	if(transform)
	{
		transform->SetPosition(XMFLOAT3(x, y, z));
		/*char buffer[128];
		sprintf(buffer, "SetPosition(%.2f, %.2f, %.2f) on actor id %i", x, y, z, GetActorId());
		GLIB_LOG("Lua", buffer);*/
	}
	else
	{
		GLIB_LOG("Lua", "No TransformComponent found");
	}
}

void ScriptComponent::SetRotation(float x, float y, float z)
{
	auto transform = MakeStrongPtr(mOwner->GetComponent<TransformComponent>(TransformComponent::g_Name));

	if(transform)
	{
		transform->SetRotation(XMFLOAT3(x, y, z));
		/*char buffer[128];
		sprintf(buffer, "SetRotation(%.2f, %.2f, %.2f) on actor id %i", x, y, z, GetActorId());
		GLIB_LOG("Lua", buffer);*/
	}
	else
	{
		GLIB_LOG("Lua", "No TransformComponent found");
	}
}

void ScriptComponent::SetScale(float x, float y, float z)
{
	auto transform = MakeStrongPtr(mOwner->GetComponent<TransformComponent>(TransformComponent::g_Name));

	if(transform)
	{
		transform->SetScale(XMFLOAT3(x, y, z));
		/*char buffer[128];
		sprintf(buffer, "SetScale(%.2f, %.2f, %.2f) on actor id %i", x, y, z, GetActorId());
		GLIB_LOG("Lua", buffer);*/
	}
	else
	{
		GLIB_LOG("Lua", "No TransformComponent found");
	}
}

void ScriptComponent::SetAlpha(float alpha)
{
	auto staticModelComponent = MakeStrongPtr(mOwner->GetComponent<StaticModelComponent>(StaticModelComponent::g_Name));

	if(staticModelComponent)
	{
		staticModelComponent->SetAlpha(alpha);
		//GLIB_LOG("Lua", string("SetAlpha() on ") + to_string(GetActorId()));
		return;
	}

	auto animatedModelComponent = MakeStrongPtr(mOwner->GetComponent<AnimatedModelComponent>(AnimatedModelComponent::g_Name));

	if(animatedModelComponent)
	{
		animatedModelComponent->SetAlpha(alpha);
		//GLIB_LOG("Lua", string("SetAlpha() on ") + to_string(GetActorId()));
		return;
	}

	GLIB_LOG("Lua", "No Static/Animated ModelComponent found");
}

LuaPlus::LuaObject ScriptComponent::GetPosition()
{
	LuaPlus::LuaObject position;

	auto transform = MakeStrongPtr(mOwner->GetComponent<TransformComponent>(TransformComponent::g_Name));
	LuaManager::Get()->ConvertVec3ToTable(transform->GetPosition(), position);

	return position;
}

LuaPlus::LuaObject ScriptComponent::GetRotation()
{
	LuaPlus::LuaObject rotation;

	auto transform = MakeStrongPtr(mOwner->GetComponent<TransformComponent>(TransformComponent::g_Name));
	LuaManager::Get()->ConvertVec3ToTable(transform->GetRotation(), rotation);

	return rotation;
}

LuaPlus::LuaObject ScriptComponent::GetScale()
{
	LuaPlus::LuaObject scale;

	auto transform = MakeStrongPtr(mOwner->GetComponent<TransformComponent>(TransformComponent::g_Name));
	LuaManager::Get()->ConvertVec3ToTable(transform->GetScale(), scale);

	return scale;
}

float ScriptComponent::GetAlpha()
{
	auto staticModelComponent = MakeStrongPtr(mOwner->GetComponent<StaticModelComponent>(StaticModelComponent::g_Name));

	if(staticModelComponent)
	{
		//GLIB_LOG("Lua", string("GetAlpha() on ") + to_string(GetActorId()));
		return staticModelComponent->GetMaterial().diffuse.w;
	}

	auto animatedModelComponent = MakeStrongPtr(mOwner->GetComponent<AnimatedModelComponent>(AnimatedModelComponent::g_Name));

	if(animatedModelComponent)
	{
		//GLIB_LOG("Lua", string("GetAlpha() on ") + to_string(GetActorId()));
		return animatedModelComponent->GetMaterial().diffuse.w;
	}

	//GLIB_LOG("Lua", "No Static/Animated ModelComponent found");
}

int ScriptComponent::GetActorId()
{
	return mOwner->GetId();
}

void ScriptComponent::SetAnimationIndex(int index, float duration)
{
	auto animatedModelComponent = MakeStrongPtr(mOwner->GetComponent<AnimatedModelComponent>(AnimatedModelComponent::g_Name));

	if(animatedModelComponent)
	{
		animatedModelComponent->SetAnimation(index, duration);
		GLIB_LOG("Lua", string("SetAnimationIndex() on ") + to_string(GetActorId()));
	}
	else
	{
		GLIB_LOG("Lua", "No AnimatedModelComponent found");
	}
}

void ScriptComponent::SetAnimationSpeed(float speed)
{
	auto animatedModelComponent = MakeStrongPtr(mOwner->GetComponent<AnimatedModelComponent>(AnimatedModelComponent::g_Name));

	if(animatedModelComponent)
	{
		animatedModelComponent->AdjustAnimationSpeedBy(speed);
		GLIB_LOG("Lua", string("SetAnimationSpeed() on ") + to_string(GetActorId()));
	}
	else
	{
		GLIB_LOG("Lua", "No AnimatedModelComponent found");
	}
}