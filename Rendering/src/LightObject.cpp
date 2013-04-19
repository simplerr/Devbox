#include "LightObject.h"
#include "BillboardManager.h"
#include "Runnable.h"
#include "Graphics.h"
#include "Light.h"
#include "World.h"

//! Graphics Library namespace.
namespace GLib
{
	//! Constructor.
	LightObject::LightObject()
		: Object3D(LIGHT_OBJECT)
	{
		// Add test billboards.
		//mBillboard = GLib::GlobalApp::GetGraphics()->AddBillboard(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(5, 5), "textures\\light_icon.png"); //[NOTE] Ugly.

		// Create the light.
		mLight = new GLib::Light();
	}

	//! Cleanup.
	LightObject::~LightObject()
	{
		// Remove the billboard.
		//mBillboard->Remove();

		// Remove the light from the world.
		if(GetWorld() != nullptr)
			GetWorld()->RemoveLight(mLight);
	}

	//! Init.
	void LightObject::Init()
	{
		// Create and add the light.
		GetWorld()->AddLight(mLight);
	}

	//! Not used.
	void LightObject::Update(float dt)
	{

	}

	//! Not used.
	void LightObject::Draw(GLib::Graphics* pGraphics)
	{
		/*XNA::AxisAlignedBox box;
		box.Center = GetPosition();
		box.Extents = XMFLOAT3(2, 2, 2);
		pGraphics->DrawBoundingBox(&box, GetWorldMatrix(), Material(XMFLOAT4(0, 255, 0, 0)));*/
	}

	//! Returns true if the ray hits lights bounding box.
	bool LightObject::RayIntersect(XMVECTOR origin, XMVECTOR direction, float& pDist)
	{
		if(XNA::IntersectRayAxisAlignedBox(origin, direction, &GetBoundingBox(), &pDist))
			return true;
		else
			return false;
	}

	//! Returns the bounding box. [NOTE] Hardcoded to (3, 3, 3).
	XNA::AxisAlignedBox LightObject::GetBoundingBox()
	{
		AxisAlignedBox box;
		box.Center = GetPosition();
		box.Extents = XMFLOAT3(3, 3, 3);
		return box;
	}

	//! Sets the material.
	void LightObject::SetMaterials(GLib::Material material)
	{
		Object3D::SetMaterials(material);
		mLight->SetMaterials(material.ambient, material.diffuse, material.specular);
	}

	//! Sets the position.
	void LightObject::SetPosition(XMFLOAT3 position)
	{
		Object3D::SetPosition(position);
		mLight->SetPosition(position);
		//mBillboard->SetPos(position);
	}

	// [NOTE] For some reason the rotation cant be (0, -1, 0) for the shadow mapping to work!
	void LightObject::SetRotation(XMFLOAT3 rotation)
	{
		Object3D::SetRotation(rotation);
		mLight->SetDirection(rotation);
	}

	//! Sets the range.
	void LightObject::SetRange(float range)
	{
		mLight->SetRange(range);
	}

	//! Sets the spot power.
	void LightObject::SetSpot(float spot)
	{
		mLight->SetSpot(spot);
	}

	//! Sets the att.
	void LightObject::SetAtt(float a0, float a1, float a2)
	{
		mLight->SetAtt(a0, a1, a2);
	}

	//! Sets the light type.
	void LightObject::SetLightType(GLib::LightType type)
	{
		mLight->SetType(type);
	}

	//! Sets the color intensity.
	void LightObject::SetIntensity(float ambient, float diffuse, float specular)
	{
		mLight->SetIntensity(ambient, diffuse, specular);
	}

	//! Returns the att.
	XMFLOAT3 LightObject::GetAtt()
	{
		return mLight->GetAtt();
	}

	//! Returns the color intensity.
	XMFLOAT3 LightObject::GetIntensity()
	{
		return mLight->GetIntensity();
	}

	//! Returns the range.
	float LightObject::GetRange()
	{
		return mLight->GetRange();
	}

	//! Returns the spot power.
	float LightObject::GetSpot()
	{
		return mLight->GetSpot();
	}

	//! Returns the light type.
	int LightObject::GetLightType()
	{
		return mLight->GetType();
	}
}	// End of Graphics Library namespace.