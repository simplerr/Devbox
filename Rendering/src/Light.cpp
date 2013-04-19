#include "Light.h"
#include "Utility.h"

//! Graphics Library namespace.
namespace GLib
{

//! Constructor.
Light::Light() 
{
	//ZeroMemory(this, sizeof(this));
	mMaterial = Colors::White;
	mType = 0;
}

void Light::SetMaterials(XMFLOAT4 ambient, XMFLOAT4 diffuse, XMFLOAT4 specular)
{
	mMaterial = Material(ambient, diffuse, specular);
	SetIntensity(1.0f, 0.0f, 0.0f);
}
	
void Light::SetPosition(XMFLOAT3 position)
{
	mPosition = position;
}
	
void Light::SetPosition(float x, float y, float z)
{
	SetPosition(XMFLOAT3(x, y, z));
}

void Light::SetRange(float range)
{
	mRange = range;
}

// [NOTE] For some reason the rotation cant be (0, -1, 0) for the shadow mapping to work!
void Light::SetDirection(XMFLOAT3 direction)
{
	// Normalize the direction.
	XMVECTOR dir = XMLoadFloat3(&direction);
	dir = XMVector3Normalize(dir);
	XMStoreFloat3(&mDirection, dir);
}
	
void Light::SetDirection(float x, float y, float z)
{
	SetDirection(XMFLOAT3(x, y, z));
}

void Light::SetSpot(float spot)
{
	mSpot = spot;
}
	
void Light::SetAtt(float a0, float a1, float a2)
{
	mAtt = XMFLOAT3(a0, a1, a2);
}
	
void Light::SetType(LightType type)
{
	mType = type;
}
 
void Light::SetIntensity(float ambient, float diffuse, float specular)
{
	mIntensity = XMFLOAT3(ambient, diffuse, specular);
}

void Light::SetId(int id)
{
	mId = id;
}

int Light::GetId()
{
	return mId;
}

XMFLOAT3 Light::GetIntensity()
{
	return mIntensity;
}

XMFLOAT3 Light::GetPosition()
{
	return mPosition;
}
	
XMFLOAT3 Light::GetDirection()
{
	return mDirection;
}
	
XMFLOAT3 Light::GetAtt()
{
	return mAtt;
}
	
float Light::GetSpot()
{
	return mSpot;
}

float Light::GetRange()
{
	return mRange;
}

Material Light::GetMaterial()
{
	return mMaterial;
}
 
int	Light::GetType()
{
	return mType;
}

}	// End of Graphics Library namespace.