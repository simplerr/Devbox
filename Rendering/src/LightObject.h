#pragma once
#include "Object3D.h"

namespace GLib {
	// Forward declarations.
	class Light;
	struct BillboardVertex;
	class World;

	//!
	//	Represents an object containing a light source.
	//!
	class LightObject : public Object3D
	{
	public:
		LightObject();
		~LightObject();

		virtual void Init();
		virtual void Update(float dt);
		virtual void Draw(Graphics* pGraphics);

		void SetMaterials(Material material);
		void SetPosition(XMFLOAT3 position);
		void SetRotation(XMFLOAT3 rotation);

		// Light functions.
		void SetRange(float range);
		void SetSpot(float spot);
		void SetAtt(float a0, float a1, float a2);
		void SetLightType(LightType type);
		void SetIntensity(float ambient, float diffuse, float specular);

		XMFLOAT3 GetAtt();
		XMFLOAT3 GetIntensity();
		float	 GetRange();
		float	 GetSpot();
		int GetLightType();

		bool RayIntersect(XMVECTOR origin, XMVECTOR direction, float& pDist);
		AxisAlignedBox GetBoundingBox();
	private:
		BillboardVertex* mBillboard;
		Light* mLight;
	};
}