#pragma once
#include "d3dUtil.h"
#include "Vertex.h"
#include <vector>
#include <string>

namespace GLib {
	//! Particle vertex.
	class Particle
	{
	public:
		Particle();
		~Particle();

		void Debug();

		void SetPosition(float x, float y, float z);
		void SetSize(float size);
		void SetInitialPos(float x, float y, float z);
		void SetInitialVelocity(float x, float y, float z);
		void SetInitialSize(float size);
		void SetInitialTime(float time);
		void SetLifetime(float lifetime);
		void SetMass(float mass);
		void SetInitialColor(float r, float g, float b, float a);

		XMFLOAT3 GetPosition();
		XMFLOAT3 GetInitialPos();
		XMFLOAT3 GetInitialVelocity();
		float GetInitPosX();
		float GetInitPosY();
		float GetInitPosZ();
		float GetInitVelX();
		float GetInitVelY();
		float GetInitVelZ();

		float GetInitialSize();
		float GetInitialTime();
		float GetLifetime();
		float GetSize();
		float GetMass();
		XMFLOAT4 GetInitialColor();

		BillboardVertex* billboard;
		XMFLOAT3	position;
		XMFLOAT3	initialPos;
		XMFLOAT3	initialVelocity;
		float       initialSize; // In pixels.
		float       initialTime;
		float       lifeTime;
		float		size;
		float       mass;
		XMFLOAT4    initialColor;
	};
}