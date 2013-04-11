#pragma once
#include <Windows.h>
#include <dinput.h>

#include <xnamath.h>
#include "d3dUtil.h"

//! Graphics Library namespace.
namespace GLib
{
	class Camera;

	//! Wrapper for input with keyboard and mouse.
	class GLIB_API Input
	{
	public:
		Input();
		~Input();

		void Update(float dt);
		void Draw();
		void Poll();

		bool KeyPressed(int key);
		bool KeyDown(int key);
		bool KeyReleased(int key);

		Ray GetWorldPickingRay(Camera *pCamera);

		XMFLOAT3 MousePosition();
		float	MouseDx();
		float	MouseDy();
		float	MouseDz();
		void	SetMousePosition(XMFLOAT3 pos);

		LRESULT MsgProc(UINT msg, WPARAM wParam, LPARAM lParam);
	private:
		unsigned char mLastKeyState[256];
		unsigned char mKeyState[256];

		IDirectInput8*       mDInput;
		IDirectInputDevice8* mMouse;
		DIMOUSESTATE2        mMouseState;
		DIMOUSESTATE2        mLastMouseState;

		XMFLOAT3 mMousePosition;
		float mDx, mDy;
	};
}