#include "CameraFPS.h"
#include "Input.h"


namespace GLib
{

	//! Constructor.
	CameraFPS::CameraFPS() 
		: Camera()
	{
		SetRotateButton(-1);
	}

	//! Cleanup.
	CameraFPS::~CameraFPS()
	{

	}

	//! Updates the camera in a FPS fashion.
	void CameraFPS::Update(Input* pInput, float dt)
	{
		// Get the look direction
		XMVECTOR direction;
		direction = XMLoadFloat3(&GetDirection());

		// Forward - backward.
		if(pInput->KeyDown('W')) 
			Move(GetDirection() * GetMovementSpeed());
		else if(pInput->KeyDown('S')) 
			Move(GetDirection() * -GetMovementSpeed());

		// Left - right.
		if(pInput->KeyDown('A')) 
			Move(GetRight() * -GetMovementSpeed());
		else if(pInput->KeyDown('D')) 
			Move(GetRight() * GetMovementSpeed());

		// Rotate the camera.
		if(mRotateButton == -1 || pInput->KeyDown(mRotateButton))
			Rotate(pInput);

		// Update the view matrix.
		UpdateViewMatrix();
	}

	//! Sets the rotate button, default -1 (no rotate button).
	void CameraFPS::SetRotateButton(int button)
	{
		mRotateButton = button;
	}

}	// End of Graphics Library namespace.