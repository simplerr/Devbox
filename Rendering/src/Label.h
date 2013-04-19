#pragma once
#include "BasicControl.h"
#include "Utility.h"

class LuaWrapper;

class Label : public BasicControl
{
public:
	Label(int x, int y, string name, string text);
	~Label();

	void Draw(GLib::Graphics* pGraphics);
	void OnLeftBtnPressed(XMFLOAT3 pos);
	void OnMouseHoover(XMFLOAT3 pos);

	void LoadLuaProperties(LuaWrapper* pLuaWrapper);

	void SetFontData(string family, int size, UINT32 color = GLib::ColorRGBA(0, 0, 0, 255));
	void SetText(string text);
	void SetCustomWidth(int width);
	void SetCentered(bool centered);

	GLib::Rect GetRect();
	string GetText();
	float GetTextWidth();
	float GetTextHeight();
private:
	FontData mFontData;
	string	 mText;
	int		 mCustomWidth;
	bool	 mCentered;
};