//#include "FontWrapper.h"
//#include "d3dUtil.h"
//
//namespace GLib {
//
//FontWrapper::FontWrapper()
//{
//
//}
//
//FontWrapper::~FontWrapper()
//{
//	// Cleanup fonts.
//	mFontFactory->Release();
//
//	for(auto iter = mFontMap.begin(); iter != mFontMap.end(); iter++)
//		(*iter).second->Release();
//}
//
//void FontWrapper::DrawText(string text, int x, int y, int size = 14, UINT32 color = 0xff000000, string family = "Arial")
//{
//	if(mFontMap.find(family) != mFontMap.end())
//	{
//		mFontMap[family]->DrawString(GetD3DContext(), StringToWString(text).c_str(), )
//	}
//	else
//	{
//		IFW1FontWrapper* font;
//		mFontFactory->CreateFontWrapper(GetD3DDevice(), StringToWString(family).c_str(), &font);
//		mFontMap[family] = font;
//	}
//}
//
//}
//
