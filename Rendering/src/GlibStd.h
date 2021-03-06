#pragma once

/******************************************************************************************//**
* @file
* @brief Include file for standard system files and files
* from the project that are used frequently, but changed
* infrequently.
* 
* @note Try to not include this in any header.
*********************************************************************************************/

// Standard includes.
#include <vector>
#include <map>
#include <list>
#include <memory>
#include <string>
#include <assert.h>
#include <Windows.h>
#include <iostream>

// DirectX includes.
#include <D3D11.h>
#include <d3dx11.h>
#include <dxerr.h>

// Project specific includes.
#include "defs.h"
#include "Logger.h"

using namespace std;

//! Graphics Library namespace.
namespace GLib
{
	// Forward declarations.
	class Light;
	class Graphics;
	class Camera;
	class Runnable;
	class Input;

	/******************************************************************************************//**
	* @brief A static class that wraps the global Runnable* instance.  
	*******************************************************************************************/
	class GLIB_API GlobalApp
	{
	public:
		static void SetRunnable(Runnable* pRunnable);

		static ID3D11Device*			GetD3DDevice();
		static ID3D11DeviceContext*		GetD3DContext();
		static Graphics*				GetGraphics();
		static Camera*					GetCamera();
		static Runnable*				GetGame();
		static Input*					GetInput();
		static HINSTANCE				GetAppInstance();
		static HWND						GetWindowHandler();
		static int						GetClientWidth();
		static int						GetClientHeight();

		static Runnable* mRunnable;
	};
}

template <class Type>
shared_ptr<Type> MakeStrongPtr(weak_ptr<Type> pWeakPtr)
{
	if (!pWeakPtr.expired())
		return shared_ptr<Type>(pWeakPtr);
	else
		return shared_ptr<Type>();
}