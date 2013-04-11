#include <sstream>
#include "Runnable.h"
#include "Graphics.h"
#include "Input.h"
#include "D3DCore.h"
#include "Camera.h"

//! Graphics Library namespace.
namespace GLib
{

LRESULT CALLBACK
MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// Don't start processing messages until the application has been created.
	/*if(GlobalApp != NULL )
		return GlobalApp->MsgProc(hwnd, msg, wParam, lParam);
	else
		return DefWindowProc(hwnd, msg, wParam, lParam);*/

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

//! Constructor. The Init() function handles the initialization.
Runnable::Runnable(HINSTANCE hInstance, std::string caption, int width, int height)
{
	// Set values.
	mGraphics		= nullptr;
	mhMainWindow	= nullptr;
	mCaption		= caption;
	mhInstance		= hInstance;
	mFullscreen		= false;
	mWindowedWidth	= width;
	mWindowedHeight	= height;
	SetFpsCap(numeric_limits<float>::infinity());
	SetUseWindowBorder(true);
}

//! Cleans up and frees all pointers.
Runnable::~Runnable()
{
	delete mGraphics;
	delete mInput;
}

//! Initializes Win32 and Direct3D by calling Graphics::Init(...).
void Runnable::Init()
{
	// Init the Win32 components.
	InitWin32();

	// Create the input member.
	mInput = new Input();

	// Init Direct3D.
	mGraphics = new Graphics();
	mGraphics->Init(GetClientWidth(), GetClientHeight(), GetHwnd(), false);
}

//! Initializes Win32 and creates the main window.
bool Runnable::InitWin32()
{
	WNDCLASS wc;
	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = MainWndProc; 
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = mhInstance;
	wc.hIcon         = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor       = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = 0;
	wc.lpszClassName = "D3DWndClassName";

	if(!RegisterClass(&wc)) {
		MessageBox(0, "RegisterClass FAILED", 0, 0);
		PostQuitMessage(0);
	}

	RECT clientRect;
	clientRect.left = GetSystemMetrics(SM_CXSCREEN)/2 - mWindowedWidth/2.0f;
	clientRect.right = GetSystemMetrics(SM_CXSCREEN)/2 + mWindowedWidth/2.0f;
	clientRect.top = GetSystemMetrics(SM_CYSCREEN)/2 - mWindowedHeight/2.0f;
	clientRect.bottom = GetSystemMetrics(SM_CYSCREEN)/2 + mWindowedHeight/2.0f;

	DWORD style = mUseBorder ? WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_CLIPCHILDREN : WS_POPUP|WS_VISIBLE|WS_SYSMENU | WS_CLIPCHILDREN;

	AdjustWindowRect(&clientRect, style, false);
	int width = clientRect.right - clientRect.left;
	int height = clientRect.bottom - clientRect.top;

	// Create the window with a custom size and make it centered
	// NOTE: WS_CLIPCHILDREN Makes the area under child windows not be displayed. (Useful when rendering DirectX and using windows controls).
	mhMainWindow = CreateWindow("D3DWndClassName", mCaption.c_str(), 
		style, GetSystemMetrics(SM_CXSCREEN)/2-(mWindowedWidth/2),
		GetSystemMetrics(SM_CYSCREEN)/2-(mWindowedHeight/2), width, height, 
		0, 0, mhInstance, 0); 

	if(!mhMainWindow) {
		auto error = GetLastError();
		MessageBox(0, "CreateWindow() failed.", 0, 0);
		PostQuitMessage(0);
	}

	// Show the newly created window.
	ShowWindow(mhMainWindow, SW_SHOW);
	UpdateWindow(mhMainWindow);

	// Return true since we succeeded.
	return true;
}

//! The game loop. Loops until it catches a WM_QUIT message.
int Runnable::Run()
{
	MSG msg = {0};
	mTimer.Reset();

	// The loop that will run until the app closes.
	while(msg.message != WM_QUIT)
	{
		// If there are window messages then process them.
		if(PeekMessage(&msg, 0, 0, 0, PM_REMOVE ))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		// Else do game stuff.
		else
		{
			// Calculate the delta time.
			mTimer.Tick();

			// Update the elapsed time.
			static float elapsedTime = 0;
			elapsedTime += mTimer.DeltaTime();

			// Limit the fps at mFpsCap.
			if(elapsedTime > 1.0f / mFpsCap) {
				CalculateFrameStats();
				mInput->Poll();
				mInput->Update(elapsedTime);
				//mGraphics->Update(mInput, elapsedTime);
				Update(mInput, elapsedTime);
				Draw(GetGraphics());
				elapsedTime = 0.0f;
			}
		}
    }

	return (int)msg.wParam;
}

//! Handles all window messages.
LRESULT Runnable::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// Let Input handle the message.
	mInput->MsgProc(msg, wParam, lParam);

	switch(msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

//! Toggle between fullscreen and windowed.
void Runnable::SwitchScreenMode()
{
	// Resize the swap chain.
	mFullscreen = !mFullscreen;
	GetD3D()->SetFullScreen(GetClientWidth(), GetClientHeight(), mFullscreen);

	// Pure virtual function.
	OnResize(GetClientWidth(), GetClientHeight());
}

void Runnable::SetFullscreen(bool fullscreen)
{
	mFullscreen = fullscreen;
	GetD3D()->SetFullScreen(GetClientWidth(), GetClientHeight(), mFullscreen);

	// Pure virtual function.
	OnResize(GetClientWidth(), GetClientHeight());
}

//! Updates the caption with information about the FPS.
void Runnable::CalculateFrameStats()
{
	// Code computes the average frames per second, and also the 
	// average time it takes to render one frame.  These stats 
	// are appended to the window caption bar.
	static int frameCnt = 0;
	static float timeElapsed = 0.0f;

	frameCnt++;

	// Compute averages over one second period.
	if((mTimer.TotalTime() - timeElapsed) >= 1.0f)
	{
		mCurrentFps = (float)frameCnt; // fps = frameCnt / 1
		float mspf = 1000.0f / mCurrentFps;

		std::ostringstream outs;   
		outs.precision(6);
		outs << mCaption << "    " << "FPS: " << mCurrentFps << "    " << "Frame Time: " << mspf << " (ms)";
		SetWindowText(GetHwnd(), outs.str().c_str());
		
		// Reset for next average.
		frameCnt = 0;
		timeElapsed += 1.0f;
	}
}

//! Returns the main HINSTANCE of the application.
HINSTANCE Runnable::GetInstance()
{
	return mhInstance;
}

//! Returns the	main HWND of the application.
HWND Runnable::GetHwnd()
{
	return mhMainWindow;
}

//! Sets the visibility of the window.
void Runnable::SetVisible(bool visible)
{
	if(visible)
		ShowWindow(mhMainWindow, SW_SHOW);
	else
		ShowWindow(mhMainWindow, SW_HIDE);
}

//! Sets the fps cap.
void Runnable::SetFpsCap(float cap)
{
	mFpsCap = cap;
}

//! Returns the window width.
int	Runnable::GetClientWidth()
{
	if(!mFullscreen)
		return mWindowedWidth;
	else
		return GetFullscreenWidth();
}

//! Returns the window height.
int	Runnable::GetClientHeight()
{
	if(!mFullscreen)
		return mWindowedHeight;
	else
		return GetFullscreenHeight();
}

//! Returns the fullscreen width.
int Runnable::GetFullscreenWidth()
{
	RECT desktop;
	GetWindowRect(GetDesktopWindow(), &desktop);
	return desktop.right;
}

//! Returns the fullscreen height.
int Runnable::GetFullscreenHeight()
{
	RECT desktop;
	GetWindowRect(GetDesktopWindow(), &desktop);
	return desktop.bottom;
}

//! Returns the Graphics D3DCore instance.
D3DCore* Runnable::GetD3D()
{
	return mGraphics->GetD3D();
}

//! Returns the Graphics instance.
Graphics* Runnable::GetGraphics()
{
	return mGraphics;
}

//! Returns the input instance.
Input* Runnable::GetInput()
{
	return mInput;
}

void Runnable::SetUseWindowBorder(bool use)
{
	mUseBorder = use;
}

void Runnable::ResizeWindow(float width, float height)
{
	mWindowedWidth = width;
	mWindowedHeight = height;

	SetWindowPos(GetHwnd(), HWND_TOP, GetSystemMetrics(SM_CXSCREEN)/2-(width/2.0f), GetSystemMetrics(SM_CYSCREEN)/2-(height/2.0f), width, height, SWP_SHOWWINDOW);
	GetD3D()->OnResize(width, height);
	OnResize(width, height);
	GetGraphics()->GetCamera()->RebuildProjection();
}

float Runnable::GetCurrentFps()
{
	return mCurrentFps;
}

}	// End of Graphics Library namespace.
