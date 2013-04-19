#include "Component.h"

Component::Component()
{
	mOwner = nullptr;
}

Component::~Component()
{
	mOwner.reset();
}

void Component::SetOwner(StrongActorPtr pOwner)
{
	mOwner = pOwner;
}

void Component::SetId(ComponentId id)
{
	mId = id;
}

ComponentId Component::GetId()
{
	return mId;
}