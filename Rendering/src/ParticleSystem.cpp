#include <cassert>
#include "ParticleSystem.h"
#include "Graphics.h"
#include "Camera.h"
#include "d3dUtil.h"
#include "Effects.h"
#include "lua_Particle.h"
#include "Particle.h"
#include "LuaWrapper.h"

namespace GLib {

ParticleSystem::ParticleSystem(XMFLOAT3 position, string luaScript, string particleTexture, float duration)
	: Object3D(PARTICLE_SYSTEM, position),
	mTime(0.0f),
	mLuaScript(luaScript)
{
	mLuaWrapper = new LuaWrapper(luaScript);

	float spawnFrequency = mLuaWrapper->Get<float>("ParticleSystemData.spawn_frequency");
	float lifetime = mLuaWrapper->Get<float>("ParticleSystemData.lifetime");
	int numMaxParticles = mLuaWrapper->Get<int>("ParticleSystemData.max_particles");
	mRadius = mLuaWrapper->Get<float>("ParticleSystemData.radius");

	SetSpawnFrequency(spawnFrequency);
	SetLifetime(lifetime);
	SetNumMaxParticles(numMaxParticles);
	mTextureName = mLuaWrapper->Get<string>("ParticleData.texture");

	if(particleTexture != "lua") 
		mTextureName = particleTexture;

	if(duration != 0.0f)
		SetLifetime(duration);

	// Allocate memory for maximum number of particles.
	mParticles.resize(mNumMaxParticles);
	mAliveParticles.reserve(mNumMaxParticles);
	mDeadParticles.reserve(mNumMaxParticles);

	// They start off all dead.
	for(int i = 0; i < mNumMaxParticles; ++i)
	{
		mParticles[i] = CreateParticle();
		mParticles[i]->SetInitialTime(0.0f);
		mParticles[i]->SetLifetime(-1);
		mParticles[i]->SetInitialPos(GetPosition().x, GetPosition().y, GetPosition().z);
		mParticles[i]->billboard = GLib::GetGraphics()->AddBillboard(GetPosition(), XMFLOAT2(1, 1), mTextureName);
		mParticles[i]->SetPosition(GetPosition().x, GetPosition().y, GetPosition().z);
	}

	SetOriginObject(nullptr);
}

ParticleSystem::~ParticleSystem()
{
	// Remove all the billboards.
	for(int i = 0; i < mNumMaxParticles; ++i) {
		GLib::GetGraphics()->RemoveBillboard(mTextureName, mParticles[i]->billboard);
		delete mParticles[i];
	}

	delete mLuaWrapper;
}

void ParticleSystem::InitLua()
{
	
}

Particle* ParticleSystem::CreateParticle()
{
	// Push arguments.
	lua_getglobal(mLuaWrapper->GetLua(), "InitParticle");
	lua_pushnumber(mLuaWrapper->GetLua(), GetPosition().x);
	lua_pushnumber(mLuaWrapper->GetLua(), GetPosition().y);
	lua_pushnumber(mLuaWrapper->GetLua(), GetPosition().z);
	lua_pushnumber(mLuaWrapper->GetLua(), mTime);
	static int counter = 0;
	lua_pushnumber(mLuaWrapper->GetLua(), counter++);

	// Call Lua function.
	int result = lua_pcall(mLuaWrapper->GetLua(), 5, 1, 0);

	// Get the return value.
	Particle* particle = (Particle*)tolua_tousertype(mLuaWrapper->GetLua(), -1, 0);

	return particle;
}

float ParticleSystem::GetTime()
{
	return mTime;
}

void  ParticleSystem::SetLifetime(float lifetime)
{
	mLifetime = lifetime;
}

void ParticleSystem::SetTime(float time)
{
	mTime = time;
}

void ParticleSystem::AddParticle()
{
	if(mDeadParticles.size() > 0)
	{
		// Reinitialize a particle.
		Particle* p = mDeadParticles.back();
		p->initialTime = mTime;

		// Get the lifetime variable from Lua.
		p->lifeTime = mLuaWrapper->GetNumber("GetLifetime");

		p->SetInitialPos(GetPosition().x, GetPosition().y, GetPosition().z);

		// No longer dead.
		mDeadParticles.pop_back();
		mAliveParticles.push_back(p);
	}
}

void ParticleSystem::Update(float dt)
{
	//if(mOriginObject != nullptr)
	//	SetPosition(mOriginObject->GetPosition());

	mTime += dt;

	if(mTime >= mLifetime)
		Kill();

	mDeadParticles.resize(0);
	mAliveParticles.resize(0);

	// For each particle.
	for(int i = 0; i < mNumMaxParticles; ++i)
	{
		Particle* particle = mParticles[i];

		// Is the particle dead?
 		if((mTime - particle->initialTime) > particle->lifeTime)
		{
			mDeadParticles.push_back(particle);

			// [HACK] Sets the position outside vision.
			float infinity = numeric_limits<float>::infinity();
			particle->billboard->SetPos(XMFLOAT3(infinity, infinity, infinity));
		}
		else
		{
			mAliveParticles.push_back(particle);

			// Run the UpdateParticle() function in Lua.
			lua_getglobal(mLuaWrapper->GetLua(), "UpdateParticle");
			tolua_pushusertype(mLuaWrapper->GetLua(), particle, "Particle");
			lua_pushnumber(mLuaWrapper->GetLua(), GetTime());
			lua_pcall(mLuaWrapper->GetLua(), 2, 0, 0);
			
			//particle->SetInitialPos(GetPosition().x, GetPosition().y, GetPosition().z);
			//particle->billboard->SetPos(particle->GetInitialPos());
			particle->billboard->SetSize(XMFLOAT2(particle->GetSize(), particle->GetSize()));
		}
	}

	// Emit particles.
	static float timeAccum = 0.0f;
	timeAccum += dt;
	while(timeAccum >= mSpawnFrequency)
	{
		AddParticle();
		timeAccum -= mSpawnFrequency;
	}
}

void ParticleSystem::SetSpawnFrequency(float frequency)
{
	mSpawnFrequency = frequency;
}

void ParticleSystem::Draw(Graphics* pGraphics)
{
	// Graphics::DrawBillboards() does the drawing.
	//pGraphics->DrawBoundingBox(&GetBoundingBox(), GetWorldMatrix(), Material(Colors::Blue));
}

void ParticleSystem::SetNumMaxParticles(int numMaxParticles)
{
	mNumMaxParticles = numMaxParticles;
}

AxisAlignedBox ParticleSystem::GetBoundingBox()
{
	AxisAlignedBox box;
	box.Center = GetPosition();
	box.Extents = XMFLOAT3(mRadius, mRadius, mRadius);
	return box;
}

void ParticleSystem::SetOriginObject(Object3D* pObject)
{
	mOriginObject = pObject;
}

Particle* ParticleSystem::GetParticle(int id)
{
	if(mAliveParticles.size() > id)
		return mAliveParticles[id];
	else
		return nullptr;
}

void ParticleSystem::SetRadius(float radius)
{
	mRadius = radius;
}

LuaWrapper* ParticleSystem::GetLuaWrapper()
{
	return mLuaWrapper;
}

void ParticleSystem::SetTexture(string texture)
{
	mTextureName = texture;
}
}
