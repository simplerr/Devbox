#include "GlibStd.h"
#include "Runnable.h"
#include "D3DCore.h"
#include "Graphics.h"

//! Graphics Library namespace.
namespace GLib
{
	//! Global Runnable* instance.
	GLIB_API Runnable* GlobalApp::mRunnable = nullptr;

	/******************************************************************************************//**
	* GlobalApp implementation. 
	*********************************************************************************************/

	//! Sets the Runnable member
	/**
	 *	@note Must be called!
	 */
	void GlobalApp::SetRunnable(Runnable* pRunnable)
	{
		mRunnable = pRunnable;
	}

	//! Returns the global Runnable instance.
	Runnable* GlobalApp::GetGame()
	{
		return mRunnable;
	}

	//! Returns the DirectX device.
	ID3D11Device* GlobalApp::GetD3DDevice()
	{
		return mRunnable->GetD3D()->GetDevice();
	}

	//! Returns the DirectX device context.
	ID3D11DeviceContext* GlobalApp::GetD3DContext()
	{
		return mRunnable->GetD3D()->GetContext();
	}

	//! Returns the Graphics component.
	Graphics* GlobalApp::GetGraphics()
	{
		return mRunnable->GetGraphics();
	}

	//! Returns the Camera component.
	Camera*	GlobalApp::GetCamera()
	{
		return mRunnable->GetGraphics()->GetCamera();
	}

	//! Returns the HINSTANCE of the app.
	HINSTANCE GlobalApp::GetAppInstance()
	{
		return mRunnable->GetInstance();
	}

	//! Returns the HWND of the window.
	HWND GlobalApp::GetWindowHandler()
	{
		return mRunnable->GetHwnd();
	}

	//! Returns the client width.
	int	GlobalApp::GetClientWidth()
	{
		return mRunnable->GetClientWidth();
	}

	//! Returns the client height.
	int GlobalApp::GetClientHeight()
	{
		return mRunnable->GetClientHeight();
	}
}

