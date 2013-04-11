#pragma once

// Linking to Rendering.lib
#ifdef _DEBUG 
#pragma comment(lib, "../debug/assimp_d.lib")
#pragma comment(lib, "../debug/Effects11_d.lib")
#pragma comment(lib, "../debug/FW1FontWrapper.lib")
#pragma comment(lib, "../debug/gwen_static_d.lib")
#pragma comment(lib, "../debug/toluapp_d.lib")

#else
#pragma comment(lib, "../debug/assimp.lib")
#pragma comment(lib, "../debug/Effects11.lib")
#pragma comment(lib, "../debug/FW1FontWrapper.lib")
#pragma comment(lib, "../debug/gwen_static.lib")
#pragma comment(lib, "../debug/toluapp.lib")
#endif

// Each class that should be exported is tagged with the GLIB_API macro
// The Rendering project has GLIB_EXPORT defined in the preprocessor definitions
#if defined(GLIB_EXPORT) // inside DLL
#   define GLIB_API   __declspec(dllexport)
#	pragma warning (disable : 4251)
#else // outside DLL
#   define GLIB_API   __declspec(dllimport)
#	pragma warning (disable : 4251)
#endif  // GLIB_EXPORT