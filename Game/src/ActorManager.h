#pragma once
#include <map>
#include "typedefs.h"

namespace GLib {
	class Graphics;
}

class ActorManager
{
public:
	typedef std::map<ActorId, StrongActorPtr> ActorMap;
public:
	~ActorManager();

	void Update(float dt);
	void Draw(GLib::Graphics* pGraphics);

	void AddActor(StrongActorPtr pActor);
private:
	ActorMap mActors;
};