#pragma once

#include <memory>
#include <string>

using namespace std;

class Actor;
class Component;

typedef unsigned int ActorId;
typedef unsigned int ComponentId;

const ActorId INVALID_ACTOR_ID = 0;
const ComponentId INVALID_COMPONENT_ID = 0;

typedef string ComponentName;
typedef shared_ptr<Component> StrongComponentPtr;
typedef shared_ptr<Actor> StrongActorPtr;
typedef string ActorType;