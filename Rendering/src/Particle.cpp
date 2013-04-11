#include "Particle.h"

namespace GLib {

Particle::Particle()
{

}

Particle::~Particle()
{

}

void Particle::SetPosition(float x, float y, float z)
{
	position = XMFLOAT3(x, y, z);
	billboard->SetPos(position);
}

void Particle::SetSize(float size)
{
	this->size = size;
}

void Particle::SetInitialPos(float x, float y, float z)
{
	initialPos = XMFLOAT3(x, y, z);
	//SetPosition(x, y, z);
}

void Particle::SetInitialVelocity(float x, float y, float z)
{
	initialVelocity = XMFLOAT3(x, y, z);
}

void Particle::SetInitialSize(float size)
{
	initialSize = size;
}

void Particle::SetInitialTime(float time)
{
	initialTime = time;
}

void Particle::SetLifetime(float lifetime)
{
	this->lifeTime = lifetime;
}

void Particle::SetMass(float mass)
{
	this->mass = mass;
}

void Particle::SetInitialColor(float r, float g, float b, float a)
{
	initialColor = XMFLOAT4(r, g, b, a);
}

XMFLOAT3 Particle::GetInitialPos()
{
	return initialPos;
}

XMFLOAT3 Particle::GetInitialVelocity()
{
	return initialVelocity;
}

float Particle::GetInitialSize()
{
	return initialSize;
}

float Particle::GetInitialTime()
{
	return initialTime;
}

float Particle::GetLifetime()
{
	return lifeTime;
}

float Particle::GetMass()
{
	return mass;
}

XMFLOAT4 Particle::GetInitialColor()
{
	return initialColor;
}

XMFLOAT3 Particle::GetPosition()
{
	return position; 
}

float Particle::GetSize()
{
	return size;
}

float Particle::GetInitPosX()
{
	return initialPos.x;
}

float Particle::GetInitPosY()
{
	return initialPos.y;
}

float Particle::GetInitPosZ()
{
	return initialPos.z;
}

float Particle::GetInitVelX()
{
	return initialVelocity.x;
}

float Particle::GetInitVelY()
{
	return initialVelocity.y;
}

float Particle::GetInitVelZ()
{
	return initialVelocity.z;
}

void Particle::Debug()
{
	OutputDebugString("debug!\n\n");
}

}

