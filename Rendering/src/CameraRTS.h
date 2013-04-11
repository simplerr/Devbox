#pragma once
#include "Camera.h"

namespace GLib
{
	//! A RTS camera.
	class GLIB_API CameraRTS : public Camera
	{
	public:
		CameraRTS();
		~CameraRTS();

		void Update(Input* pInput, float dt);
	private:
	};
}