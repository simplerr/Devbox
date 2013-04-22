#include "ActorManager.h"
#include "Actor.h"

ActorManager::~ActorManager()
{
	Clear();
}

void ActorManager::Update(float dt)
{
	// Update game actors.
	for (ActorMap::const_iterator it = mActors.begin(); it != mActors.end(); ++it)
	{
		it->second->Update(dt);
	}
}

void ActorManager::Draw(GLib::Graphics* pGraphics)
{
	// Draw game actors.
	for (ActorMap::const_iterator it = mActors.begin(); it != mActors.end(); ++it)
	{
		it->second->Draw(pGraphics);
	}
}

void ActorManager::AddActor(StrongActorPtr pActor)
{
	mActors.insert(std::make_pair(pActor->GetId(), pActor));
}

void ActorManager::Clear()
{
	// Destroy all actors.
	for (auto it = mActors.begin(); it != mActors.end(); ++it)
		it->second->Destroy();

	mActors.clear();
}
