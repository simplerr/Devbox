#pragma once
#include "Camera.h"

namespace GLib
{
	//! A FPS camera.
	class GLIB_API CameraFPS : public Camera
	{
	public:
		CameraFPS();
		~CameraFPS();

		void Update(Input* pInput, float dt);
		void SetRotateButton(int button);
	private:
		int mRotateButton;
	};
}