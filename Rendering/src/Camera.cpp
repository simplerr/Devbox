#include "Camera.h"
#include "Input.h"
#include "Graphics.h"
#include "Runnable.h"

//! Graphics Library namespace.
namespace GLib
{

	//! Constructor.
	Camera::Camera()
	{
		// Set the sensitivity and speed
		SetLookSensitivity(2.0f);
		SetMovementSpeed(0.6f);
		SetLocked(false);

		// Default position and target
		mPosition	= XMFLOAT3(-30, 60, 0);
		mTarget		= XMFLOAT3(0.0f, 0.0f, 0.0f);
		mUp			= XMFLOAT3(0.0f, 1.0f, 0.0f);		// Weird up vector

		// Calculate the new direction.
		UpdatePitchYaw();

		// Build the projection matrix
		XMMATRIX proj = XMMatrixPerspectiveFovLH(XM_PI * 0.25, (float)GlobalApp::GetClientWidth()/(float)GlobalApp::GetClientHeight(), 1.0f, 1000.0f);
		XMStoreFloat4x4(&mProj, proj);

		UpdateViewMatrix();
		
		// Build the camera frustum.
		mFrustum.ConstructFromProjection(GetProjectionMatrix());
		//XNA::ComputeFrustumFromProjection(&mFrustum, &XMLoadFloat4x4(&GetProjectionMatrix()));
	}

	//! Cleanup.
	Camera::~Camera()
	{

	}

	void Camera::RebuildProjection()
	{
		// Build the projection matrix
		XMMATRIX proj = XMMatrixPerspectiveFovLH(XM_PI * 0.25, (float)GlobalApp::GetClientWidth()/(float)GlobalApp::GetClientHeight(), 1.0f, 1000.0f);
		XMStoreFloat4x4(&mProj, proj);
	}

	//! Updates the view matrix.
	void Camera::UpdateViewMatrix()
	{
		// Update the right vector.
		XMVECTOR up = XMLoadFloat3(&mUp);
		XMVECTOR dir = XMLoadFloat3(&GetDirection());
		XMVECTOR right = XMVector3Cross(up, dir);
		right = XMVector3Normalize(right);
		XMStoreFloat3(&mRight, right);

		// Update the view matrix
		XMVECTOR pos = XMLoadFloat3(&mPosition);
		XMVECTOR target = XMLoadFloat3(&mTarget);
		XMStoreFloat4x4(&mView, XMMatrixLookAtLH(pos, target, up));

		XMFLOAT4X4 viewProj;
		XMStoreFloat4x4(&viewProj, XMLoadFloat4x4(&GetViewMatrix()) * XMLoadFloat4x4(&GetProjectionMatrix()));
		mFrustum.BuildFromViewProjection(viewProj);
	}

	void Camera::Move(XMFLOAT3 speed)
	{
		mPosition = mPosition + speed;
		mTarget = mTarget + speed;
	}

	//! Reads mouse movement and rotates the camera correspondingly.
	void Camera::Rotate(Input* pInput)
	{
		// Increase the pitch and yaw angles.
		mPitch += pInput->MouseDy() / (-13.0f / mSensitivity);
		mYaw += pInput->MouseDx() / (13.0f / mSensitivity);

		// Limit to PI/2 radians.
		if(mPitch > 0)
			mPitch = min(mPitch, 1.54f);
		else
			mPitch = max(mPitch, -1.54f);

		// Calculate the new direction.
		XMVECTOR direction;
		float r = cosf(mPitch);
		direction = XMVectorSet(r * sinf(mYaw), sinf(mPitch), r * cosf(mYaw), 0);

		// Set the new target.
		XMStoreFloat3(&mTarget, XMLoadFloat3(&mPosition) + direction);
	}

	//! Outputs debug information in the Output window.
	void Camera::DrawDebug()
	{
		char buffer[256];
		sprintf(buffer, "Pos: (%f, %f, %f)\n", mPosition.x, mPosition.y, mPosition.z);
		OutputDebugString(buffer);
		sprintf(buffer, "Up: (%f, %f, %f)\nTarget: (%f, %f, %f)\n", mUp.x, mUp.y, mUp.z, mTarget.x, mTarget.y, mTarget.z);
		OutputDebugString(buffer);
		sprintf(buffer, "[Direction] x: %f, y: %f, z: %f\n", GetDirection().x, GetDirection().y, GetDirection().z);
		OutputDebugString(buffer);
		sprintf(buffer, "Yaw: %f Pitch: %f\n", mYaw, mPitch);
		OutputDebugString(buffer);
	}

	//! Sets the cameras position.
	/**
	@note make sure to call SetTarget() after this if you want to aim at the same target
	*/
	void Camera::SetPosition(XMFLOAT3 position)
	{
		mPosition = position;
		UpdatePitchYaw();
		UpdateViewMatrix();
	}

	//! Sets the cameras direction.
	void Camera::SetDirection(XMFLOAT3 direction)
	{
		mTarget = mPosition + direction;
		UpdatePitchYaw();
	}

	//! Sets the point the camera will aim at.
	void Camera::SetTarget(XMFLOAT3 target)
	{
		mTarget = target;
		UpdatePitchYaw();
	}

	//! Updates the yaw and pitch members.
	void Camera::UpdatePitchYaw()
	{
		// Calculate new yaw and pitch.
		XMFLOAT3 direction = GetDirection();
		mPitch = asinf(direction.y);
		mYaw = atan2f(direction.x, direction.z);
	}

	//! Sets the look sensitivity.
	void Camera::SetLookSensitivity(float sensitivity)
	{
		mSensitivity = sensitivity/100.0f;
	}

	//! Sets the movement speed of the camera.
	void Camera::SetMovementSpeed(float speed)
	{
		mMovementSpeed = speed;
	}

	//! Returns the view matrix.
	XMFLOAT4X4 Camera::GetViewMatrix()
	{
		return mView;
	}

	//! Returns the projection matrix.
	XMFLOAT4X4 Camera::GetProjectionMatrix()
	{
		return mProj;
	}

	//! Returns the movement speed.
	float Camera::GetMovementSpeed()
	{
		return mMovementSpeed;
	}

	//! Returns the cameras position.
	XMFLOAT3 Camera::GetPosition()
	{
		return mPosition;
	}

	//! Returns the cameras target.
	XMFLOAT3 Camera::GetTarget()
	{
		return mTarget;
	}

	//! Returns the right vector.
	XMFLOAT3 Camera::GetRight()
	{
		return mRight;
	}

	//! Retuns the cameras looking direction.
	XMFLOAT3 Camera::GetDirection()
	{
		XMVECTOR direction;
		direction = XMVector3Normalize(XMLoadFloat3(&(mTarget - mPosition)));
		XMFLOAT3 dir;
		XMStoreFloat3(&dir, direction);

		return dir;
	}

	//! Returns the camera frustum in world space.
	GLib::Frustum Camera::GetFrustum()
	{
		return mFrustum.GetWorldFrustum(GetViewMatrix());
	}

	//! Returns the ray from the camera origin in world space.
	Ray Camera::GetWorldPickingRay()
	{
		Ray ray = {mPosition, GetDirection()};
		return ray;
	}

	//! Sets the yaw. [NOTE] Not sure if this works.
	void Camera::SetYaw(float yaw)
	{
		mYaw = yaw;
		UpdateViewMatrix();
	}

	//! Sets the pitch. [NOTE] Not sure if this works.
	void Camera::SetPitch(float pitch)
	{
		mPitch = pitch;
		UpdateViewMatrix();
	}

	void Camera::SetLocked(bool locked)
	{
		mLocked = locked;
	}

	bool Camera::GetLocked()
	{
		return mLocked;
	}
}	// End of Graphics Library namespace.