#include "CameraRTS.h"
#include "Input.h"

namespace GLib
{

	//! Constructor.
	CameraRTS::CameraRTS() 
		: Camera()
	{
		SetMovementSpeed(0.6f);
	}

	//! Cleanup.
	CameraRTS::~CameraRTS()
	{

	}

	//! Updates the camera in a RTS fashion.
	void CameraRTS::Update(Input* pInput, float dt)
	{
		if(GetLocked())
			return;

		XMFLOAT3 mousePos = pInput->MousePosition();

		// Temp [NOTE]
		//if(pInput->KeyDown(VK_MBUTTON)) {
			int scrollBarSize = 5.0f;
			if(mousePos.x < scrollBarSize || mousePos.x > 65000) // [NOTE] For some reason mousePos.x can be 65 534 when outside the window.
				Move(XMFLOAT3(0, 0, 1) * GetMovementSpeed());
			else if(mousePos.x > GlobalApp::GetClientWidth() - scrollBarSize)
				Move(XMFLOAT3(0, 0, -1) * GetMovementSpeed());

			if(mousePos.y < scrollBarSize)
				Move(XMFLOAT3(1, 0, 0) * GetMovementSpeed());
			else if(mousePos.y > GlobalApp::GetClientHeight() - scrollBarSize)
				Move(XMFLOAT3(-1, 0, 0) * GetMovementSpeed());

			Move(GetDirection() * pInput->MouseDz() * 0.10f);

			// Update the view matrix.
			UpdateViewMatrix();
		//}
	}

}	// End of Graphics Library namespace.