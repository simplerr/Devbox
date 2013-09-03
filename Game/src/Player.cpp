#include "Player.h"
#include "Graphics.h"
#include "Camera.h"

Player::Player(GLib::ModelImporter* importer, string filename)
	: GLib::Object3D(GLib::ObjectType::ANY)
	//: GLib::AnimatedObject(importer, filename)
{

}

Player::~Player()
{

}

void Player::Init()
{

}

void Player::Update(float dt)
{


	//AnimatedObject::Update(dt);
}

void Player::Draw(GLib::Graphics* pGraphics)
{
	pGraphics->GetCamera()->SetPosition(GetPosition());

	//pGraphics->DrawBoundingBox(GetPosition(), 2, 2, 2);

	//AnimatedObject::Draw(pGraphics);
}

bool Player::RayIntersect(XMVECTOR origin, XMVECTOR direction, float& pDist)
{
	return false;
}

AxisAlignedBox Player::GetBoundingBox()
{
	return AxisAlignedBox();
}