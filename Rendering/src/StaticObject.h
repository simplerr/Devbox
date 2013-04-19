#pragma once
#include "Object3D.h"

namespace GLib {
	// Forward declarations.
	class World;

	//!
	//	Represents a static object. 
	//	Can be loaded from a file or the primitive factory.
	//!
	class GLIB_API StaticObject : public Object3D
	{
	public:
		StaticObject(ModelImporter* importer, string filename);
		~StaticObject();

		virtual void Init();
		virtual void Update(float dt);
		virtual void Draw(Graphics* pGraphics);

		bool RayIntersect(XMVECTOR origin, XMVECTOR direction, float& pDist);
		AxisAlignedBox GetBoundingBox();
		StaticModel* GetModel();
		string GetFilename();
	private:
		StaticModel* mModel;
	};
}