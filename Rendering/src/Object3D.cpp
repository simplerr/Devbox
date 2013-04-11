#include "Object3D.h"
#include "Graphics.h"
#include "Effects.h"
#include "Runnable.h"
#include "Primitive.h"
#include "PrimitiveFactory.h"
#include "D3DCore.h"
#include "StaticModel.h"
#include "StaticMesh.h"
#include "ModelImporter.h"

//! Graphics Library namespace.
namespace GLib
{
	//! Constructor.
	Object3D::Object3D(ObjectType type, XMFLOAT3 position, XMFLOAT3 rotation, XMFLOAT3 scale)
	{
		SetPosition(position);
		SetRotation(rotation);
		SetScale(scale);
		SetBoundingBoxVisible(false);
		SetName("Object");
		SetDefualtScale(1.0f);
		SetMaterials(GLib::Colors::White);
		SetId(-1);
		SetType(type);
		SetWorld(nullptr);
		mAlive = true;
	}

	//! Cleanup.
	Object3D::~Object3D()
	{

	}

	//! Sets the position.
	void Object3D::SetPosition(XMFLOAT3 position)
	{
		mPosition = position;
	}

	//! Sets the rotation.
	void Object3D::SetRotation(XMFLOAT3 rotation)
	{
		mRotation = rotation;
	}

	//! Sets the scale.
	void Object3D::SetScale(XMFLOAT3 scale)
	{
		scale.x = max(0, scale.x);
		scale.y = max(0, scale.y);
		scale.z = max(0, scale.z);
		mScale = scale;
	}

	//! Sets the world.
	void Object3D::SetWorld(World* pWorld)
	{
		mWorld = pWorld;
	}

	//! Sets the default orientation.
	void Object3D::SetDefaultOrientation()
	{
		SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
		SetRotation(XMFLOAT3(0.0f, 0.0f, 0.0f));
		SetScale(XMFLOAT3(1.0f, 1.0f, 1.0f));
	}

	//! Returns the world the object is in.
	World* Object3D::GetWorld()
	{
		return mWorld;
	}

	//! Returns the world matrix.
	XMMATRIX Object3D::GetWorldMatrix()
	{
		XMMATRIX T, R, S, W;
		T = XMMatrixTranslation(mPosition.x, mPosition.y, mPosition.z);
		R = XMMatrixRotationRollPitchYaw(mRotation.x, mRotation.y, mRotation.z);
		S = XMMatrixScaling(mScale.x * mDefaultScale.x, mScale.y * mDefaultScale.y, mScale.z * mDefaultScale.z);

		return S*R*T;
	}

	//! Returns the position.
	XMFLOAT3 Object3D::GetPosition()
	{
		return mPosition;
	}

	//! Returns the rotation.
	XMFLOAT3 Object3D::GetRotation()
	{
		return mRotation;
	}

	//! Returns the scale.
	XMFLOAT3 Object3D::GetScale()
	{
		return mScale;

	}

	//! Returns the material.
	GLib::Material Object3D::GetMaterial()
	{
		return mMaterial;
	}

	//! Sets the object type.
	void Object3D::SetType(ObjectType type)
	{
		mType = type;
	}

	//! Sets the material.
	void Object3D::SetMaterials(GLib::Material material)
	{
		mMaterial = material;
	}

	//! Returns the object type.
	ObjectType Object3D::GetType()
	{
		return mType;
	}
 
	//! Returns the object name.
	string Object3D::GetName()
	{
		return mName;
	}

	//! True if the bounding box should be drawn.
	void Object3D::SetBoundingBoxVisible(bool visible)
	{
		mDrawBoundingBox = visible;
	}

	//! Returns true if the bounding box is visible.
	bool Object3D::IsBoundingBoxVisible()
	{
		return mDrawBoundingBox;
	}

	//! Sets the object name.
	void Object3D::SetName(string name)
	{
		mName = name;
	}

	//! Sets the default scale.
	void Object3D::SetDefualtScale(float scale)
	{
		mDefaultScale = XMFLOAT3(scale, scale, scale);
	}

	//! Returns true if the object is alive.
	bool Object3D::GetAlive()
	{
		return mAlive;
	}

	//! Kill the object, gets removed from World next frame.
	void Object3D::Kill()
	{
		mAlive = false;
	}

	//! Set the id.
	void Object3D::SetId(int id)
	{
		mId = id;
	}

	//! Returns the id.
	int	Object3D::GetId()
	{
		return mId;
	}
}	// End of Graphics Library namespace.