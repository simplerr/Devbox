#pragma once

// Linking to 3d party tools
#ifdef _DEBUG 
#pragma comment(lib, "../debug/assimp_d.lib")
#pragma comment(lib, "../debug/Effects11_d.lib")
#pragma comment(lib, "../debug/FW1FontWrapper.lib")
#pragma comment(lib, "../debug/gwen_static_d.lib")
#pragma comment(lib, "../debug/luaplus51-1201_debug.lib")
#else
#pragma comment(lib, "../release/assimp.lib")
#pragma comment(lib, "../release/Effects11.lib")
#pragma comment(lib, "../release/FW1FontWrapper.lib")
#pragma comment(lib, "../release/gwen_static.lib")
#pragma comment(lib, "../release/toluapp.lib")
#endif

/******************************************************************************************//**
* Each class that should be exported is tagged with the GLIB_API macro
* The Rendering project has GLIB_EXPORT defined in the preprocessor definitions
*********************************************************************************************/
#if defined(GLIB_EXPORT) // inside DLL
#   define GLIB_API   __declspec(dllexport)
#	pragma warning (disable : 4251)
#else // outside DLL
#   define GLIB_API   __declspec(dllimport)
#	pragma warning (disable : 4251)
#endif  // GLIB_EXPORT

/******************************************************************************************//**
* Debug macro that catches HRESULT errors. 
*********************************************************************************************/
#if defined(DEBUG) | defined(_DEBUG)
#ifndef HR
#define HR(x)                                              \
		{                                                          \
		HRESULT hr = (x);                                      \
		if(FAILED(hr))                                         \
			{                                                      \
			DXTrace(__FILE__, (DWORD)__LINE__, hr, #x, true); \
			}                                                      \
		}
#endif

#else
#ifndef HR
#define HR(x) (x)
#endif
#endif 

//!  Macro for releasing COM objects.
#define ReleaseCOM(x) { if(x){ x->Release(); x = 0; } }

//! A ninja define to avoid linking problems.
#define GLOBALCONST extern CONST __declspec(selectany)