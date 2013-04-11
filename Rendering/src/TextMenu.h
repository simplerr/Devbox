#pragma once
#include "BasicControl.h"
#include <vector>
#include <boost\function.hpp>
#include <boost\bind.hpp>

class Label;

class TextMenu : public BasicControl
{
public:
	TextMenu(float x, float y, string name);
	~TextMenu();

	virtual void Update(GLib::Input* pInput, float dt);
	virtual void Draw(GLib::Graphics* pGraphics);
	void AddItem(Label* pItem);
	void AddItem(string name, string text);
	void LoadLuaProperties(LuaWrapper* pLuaWrapper);
	void PerformLayout();
	void DeactivateAllItems();
	void ClearItems();

	void OnLeftBtnPressed(XMFLOAT3 pos);
	void OnMouseHoover(XMFLOAT3 pos);

	void SetFontData(string family, int size, UINT32 color);
	void SetSpacing(float spacing);
	void SetBkgdScale(float scale);
	void SetCenteredItems(bool centered);
	void SetTextColor(UINT32 defaultColor, UINT32 highlightColor);
	void SetPressedSound(string filename);

	GLib::Rect GetRect();
	float GetBkgdScale();

	// Callback hookups.
	template <class T>
	void AddItemPressedListener(void(T::*_callback)(Label*), T* _object)	{
		OnItemPressed = boost::bind(_callback, _object, _1);
	}
private:
	boost::function<void(Label*)> OnItemPressed;
private:
	FontData mFontData;
	vector<Label*> mItemList;
	UINT32 mHighlightColor;
	float mBkgdScale;
	float mSpacing;
	bool mCenteredItems;
	string mSoundEffect;
};