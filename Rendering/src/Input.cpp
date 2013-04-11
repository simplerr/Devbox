#include "Input.h"
#include "Runnable.h"
#include "Graphics.h"
#include "Camera.h"
#include "D3DCore.h"
#include "d3dUtil.h"

//! Graphics Library namespace.
namespace GLib
{

//! Constructor.
Input::Input()
{
	// Set every key/button 0.
	ZeroMemory(mLastKeyState, sizeof(mLastKeyState));
	ZeroMemory(mKeyState, sizeof(mKeyState));

	HR(DirectInput8Create(GlobalApp::GetAppInstance(), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&mDInput, 0));

	// Get the cursor starting position.
	POINT mousePosition;
	GetCursorPos(&mousePosition);
	mMousePosition.x = mousePosition.x;
	mMousePosition.y = mousePosition.y;

	HR(mDInput->CreateDevice(GUID_SysMouse, &mMouse, 0));
	HR(mMouse->SetDataFormat(&c_dfDIMouse2));
	HR(mMouse->SetCooperativeLevel(GlobalApp::GetWindowHandler(), DISCL_NONEXCLUSIVE|DISCL_FOREGROUND));
	HR(mMouse->Acquire());

	// No delta movement to start with
	mDx = mDy = 0.0f;
}

//! Cleanup.
Input::~Input()
{
	ReleaseCOM(mDInput);
	mMouse->Unacquire();
	ReleaseCOM(mMouse);
}

//! Update the key state.
/**
@param dt The delta time since the last frame.
*/
void Input::Update(float dt)
{
	// Set the old states.
	memcpy(mLastKeyState, mKeyState, sizeof(mKeyState));

	// Get the current keyboard state.
	GetKeyboardState(mKeyState);

	POINT point;
	GetCursorPos(&point);
		SetMousePosition(XMFLOAT3(point.x, point.y, 0));
}

//! Draws debug information.
void Input::Draw()
{

}

//! Updates the mouse position.
/**
@param msg The message.
@param wParam Message data.
@param lParam Message data.
*/
LRESULT Input::MsgProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch( msg )
	{
	case WM_MOUSEMOVE:	{
		//SetMousePosition(XMFLOAT3(LOWORD(lParam), HIWORD(lParam), 0));
		return 0;
		}
	case WM_LBUTTONDOWN:	{
		//showMsg("doo");
		return 0;
		}
	}

	return true;
}

//! Checks if the key was pressed.
/**
@param key The to check.
@return True if pressed.
*/
bool Input::KeyPressed(int key)
{
	if(key > 255 || key < 0)
		return false;

	// Usees bitwise AND to remove the least significant bit which is set if the key was pressed after the previous call to GetAsyncKeyState.
	// See http://msdn.microsoft.com/en-us/library/windows/desktop/ms646293(v=VS.85).aspx for more info.
	// The same thing in keyDown() and keyReleased()
	return (mKeyState[key] & 0x80) && !(mLastKeyState[key] & 0x80);
}

//! Checks if the key is down.
/**
@param key The to check.
@return True if down.
*/
bool Input::KeyDown(int key)
{
	if(key > 255 || key < 0)
		return false;

	return (bool)(mKeyState[key] & 0x80);
}

//! Checks if the key was released.
/**
@param key The to check.
@return True if released.
*/
bool Input::KeyReleased(int key)
{
	if(key > 255 || key < 0)
		return false;

	return !(mKeyState[key] & 0x80) && (mLastKeyState[key] & 0x80);
}

//! Get the mouse position
/**
@return The mouse position.
*/
XMFLOAT3 Input::MousePosition()
{
	XMFLOAT3 position = mMousePosition;
	//position.x /= GetGraphics()->GetD3D()->GetDimensionRatio().right;
	//position.y /= GetGraphics()->GetD3D()->GetDimensionRatio().bottom;

	return position;

	//POINT pos;
	//GetCursorPos(&pos);
	//ScreenToClient(GetWindowHandler(), &pos);

	//// [NOTE] Doesn't use the other code..
	////return XMFLOAT3(pos.x, pos.y, 0);

	//RECT r;
	//GetWindowRect(GetWindowHandler(), &r);
	//float width = r.right - r.left;
	//float height = r.bottom - r.top;

	//GetClientRect(GetWindowHandler(), &r);
	//XMFLOAT3 position = mMousePosition;
	//position.x = (mMousePosition.x / (float)r.right) * width;
	//position.y = (mMousePosition.y / (float)r.bottom) * height;

	//position.x /= GetGraphics()->GetD3D()->GetDimensionRatio().right;
	//position.y /= GetGraphics()->GetD3D()->GetDimensionRatio().bottom;

	//return position;
}

//! Set the mouse position.
/**
@param pos The new position.
@note Doesn't acctually change the mouse position on the screen, just the data.
*/
void Input::SetMousePosition(XMFLOAT3 pos)
{
	mDx = pos.x - MousePosition().x;
	mDy = pos.y - MousePosition().y;

	mMousePosition = pos;
}

//! Returns horizontal delta movement.
float Input::MouseDx()
{
	return (float)mMouseState.lX;
}

//! Returns vertical delta movement.
float Input::MouseDy()
{
	return (float)mMouseState.lY;
}

float Input::MouseDz()
{
	return (float)mMouseState.lZ;
}

void Input::Poll()
{
	// Poll mouse.
	memcpy(&mLastMouseState, &mMouseState, sizeof(mMouseState));
	HRESULT hr = mMouse->GetDeviceState(sizeof(DIMOUSESTATE2), (void**)&mMouseState); 
	if( FAILED(hr) )
	{
		// Mouse lost, zero out mouse data structure.
		ZeroMemory(&mMouseState, sizeof(mMouseState));

		// Try to acquire for next time we poll.
		hr = mMouse->Acquire(); 
	}
}

Ray Input::GetWorldPickingRay(Camera* pCamera)
{
	XMFLOAT3 mousePos = MousePosition();
	XMMATRIX proj = XMLoadFloat4x4(&pCamera->GetProjectionMatrix());

	// Compute the ray in view space.
	float vx = (+2.0f * mousePos.x / GlobalApp::GetClientWidth() - 1) / proj(0, 0);
	float vy = (-2.0f * mousePos.y / GlobalApp::GetClientHeight() + 1) / proj(1, 1);

	XMVECTOR origin = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	XMVECTOR dir = XMVectorSet(vx, vy, 1.0f, 0.0f);

	// Transform the ray to world space with the inverse view matrix.
	XMVECTOR det;
	XMMATRIX view = XMLoadFloat4x4(&pCamera->GetViewMatrix());
	XMMATRIX invView = XMMatrixInverse(&det, view);

	origin = XMVector3TransformCoord(origin, invView);
	dir = XMVector3TransformNormal(dir, invView);

	// Normalize the direction.
	dir = XMVector3Normalize(dir);

	Ray ray;
	XMStoreFloat3(&ray.origin, origin);
	XMStoreFloat3(&ray.direction, dir);

	return ray;
}

}	// End of Graphics Library namespace.