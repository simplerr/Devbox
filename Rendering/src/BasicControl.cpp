#include "BasicControl.h"
#include "Graphics.h"
#include "Utility.h"

BasicControl::BasicControl(float x, float y, string name)
{
	SetPosition(x, y);
	SetName(name);
	SetParent(nullptr);
	SetDrawBkgd(false);
	SetVisible(true);
	SetBkgdScale(1.0f);
	mBkgdTexture = nullptr;
}

BasicControl::~BasicControl()
{

}

void BasicControl::SetActivated(bool activated)
{
	mActivated = activated;
}

bool BasicControl::GetActivated()
{
	return mActivated;
}

void BasicControl::SetPosition(float x, float y)
{
	mPosition = XMFLOAT2(x, y);
}

XMFLOAT2 BasicControl::GetPosition()
{
	if(GetParent() != nullptr)
		return mPosition + GetParent()->GetPosition();
	else
		return mPosition;
}

void BasicControl::SetName(string name)
{
	mName = name;
}

string BasicControl::GetName()
{
	return mName;
}

void BasicControl::SetBkgdTexture(string textureName)
{
	mBkgdTexture = GLib::GlobalApp::GetGraphics()->LoadTexture(textureName);
}

void BasicControl::SetDrawBkgd(bool drawBkgd)
{
	mDrawBkgdTexture = drawBkgd;
}

BasicControl* BasicControl::GetParent()
{
	return mParent;
}

void BasicControl::SetParent(BasicControl* pParent)
{
	mParent = pParent;
}

bool BasicControl::PointInsideControl(XMFLOAT3 point)
{
	GLib::Rect rect = GetRect();

	return (point.x > rect.left && point.x < rect.right && point.y > rect.top && point.y < rect.bottom);
}

bool BasicControl::IsDrawingBkgd()
{
	return mDrawBkgdTexture;
}

GLib::Texture2D* BasicControl::GetBkgdTexture()
{
	return mBkgdTexture;
}

float BasicControl::GetBkgdScale()
{
	return mBkgdScale;
}

void BasicControl::SetBkgdScale(float scale)
{
	mBkgdScale = scale;
}

ControlAlignment BasicControl::GetAlignment()
{
	return mAlignment;
}

void BasicControl::SetAlignment(bool fixedX, bool fixedY)
{
	mAlignment.fixedX = fixedX;
	mAlignment.fixedY = fixedY;

}

bool BasicControl::GetVisible()
{
	return mVisible;
}

void BasicControl::SetVisible(bool visible)
{
	mVisible = visible;
}