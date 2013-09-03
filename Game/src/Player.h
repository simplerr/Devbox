#pragma once
#include "AnimatedObject.h"

class Player : public GLib::Object3D
{
public:
	Player(GLib::ModelImporter* importer, string filename);
	virtual ~Player();

	virtual void Init();
	virtual void Update(float dt);
	virtual void Draw(GLib::Graphics* pGraphics);

	virtual bool RayIntersect(XMVECTOR origin, XMVECTOR direction, float& pDist);
	virtual AxisAlignedBox GetBoundingBox();
private:

};
