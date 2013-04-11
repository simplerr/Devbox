#include "TextMenu.h"
#include "Label.h"
#include "Graphics.h"
#include "LuaWrapper.h"
#include "d3dUtil.h"
#include "Sound.h"

TextMenu::TextMenu(float x, float y, string name)
	: BasicControl(x, y, name)
{
	SetBkgdTexture("data/textures/snow.dds");
	SetDrawBkgd(true);
	SetFontData("Arial", 30, GLib::ColorRGBA(0, 0, 0, 255));
	SetSpacing(20.0f);
	SetCenteredItems(false);
	SetTextColor(GLib::ColorRGBA(0, 0, 0, 255), GLib::ColorRGBA(255, 255, 255, 255));
	SetPressedSound("-none-");
}

TextMenu::~TextMenu()
{
	for(int i = 0; i < mItemList.size(); i++)
		delete mItemList[i];

	mItemList.clear();
}

void TextMenu::Update(GLib::Input* pInput, float dt)
{
	DeactivateAllItems();
}

void TextMenu::Draw(GLib::Graphics* pGraphics)
{
	if(IsDrawingBkgd()) {
		GLib::Rect rect = GetRect();
		pGraphics->DrawScreenQuad(GetBkgdTexture(), GetPosition().x , GetPosition().y + rect.Height()*mBkgdScale/2.0f - mItemList[0]->GetRect().Height()/2, rect.Width()*mBkgdScale, rect.Height()*mBkgdScale); // [NOTE] -20
	}

	for(int i = 0; i < mItemList.size(); i++)
		mItemList[i]->Draw(pGraphics);
}

void TextMenu::AddItem(Label* pItem)
{
	pItem->SetFontData(mFontData.family, mFontData.size, mFontData.color);
	pItem->SetParent(this);
	mItemList.push_back(pItem);

	//PerformLayout();
}

void TextMenu::AddItem(string name, string text)
{
	Label* label = new Label(0, 0, name, text);
	AddItem(label);
}

void TextMenu::LoadLuaProperties(LuaWrapper* pLuaWrapper)
{
	float x = pLuaWrapper->Get<float>(GetName() + ".pos_x");
	float y = pLuaWrapper->Get<float>(GetName() + ".pos_y");
	float spacing = pLuaWrapper->Get<float>(GetName() + ".spacing");
	float fontSize = pLuaWrapper->Get<float>(GetName() + ".font_size");
	float bkgdScale = pLuaWrapper->Get<float>(GetName() + ".bkgd_scale");
	string fontFamily = pLuaWrapper->Get<string>(GetName() + ".font_family");
	string color = pLuaWrapper->Get<string>(GetName() + ".font_color");
	string itemBkgd = pLuaWrapper->Get<string>(GetName() + ".item_bkgd");
	string menuBkgd = pLuaWrapper->Get<string>(GetName() + ".menu_bkgd");
	int drawBkgd = pLuaWrapper->Get<float>(GetName() + ".draw_bkgd");
	int drawItemBkgd = pLuaWrapper->Get<float>(GetName() + ".draw_item_bkgd");
	int centeredItems = pLuaWrapper->Get<float>(GetName() + ".centered_items");
	float itemBkgdScale = pLuaWrapper->Get<float>(GetName() + ".item_bkgd_scale");

	SetBkgdScale(bkgdScale);
	SetBkgdTexture(menuBkgd);
	SetDrawBkgd(drawBkgd == 1 ? true : false);
	SetPosition(x, y);
	SetSpacing(spacing);
	SetCenteredItems((bool)centeredItems);
	SetFontData(fontFamily, fontSize, GLib::StripRGBA(color));

	if(drawItemBkgd)
	{
		for(int i = 0; i < mItemList.size(); i++) {
			mItemList[i]->SetBkgdTexture(itemBkgd);
			mItemList[i]->SetBkgdScale(itemBkgdScale);
		}
	}

	PerformLayout();
}

void TextMenu::PerformLayout()
{
	// Find the widest menu item.
	float maxWidth = -1;
	for(int i = 0; i < mItemList.size(); i++) {
		mItemList[i]->SetCustomWidth(0);

		if(mItemList[i]->GetRect().Width() > maxWidth)
			maxWidth = mItemList[i]->GetRect().Width();
	}

	if(mItemList.size() != 0)
	{
		mItemList[0]->SetCentered(mCenteredItems);
		mItemList[0]->SetPosition(0, 0);
		mItemList[0]->SetCustomWidth(maxWidth);

		for(int i = 1; i < mItemList.size(); i++)
		{
			float lastHeight = mItemList[i-1]->GetRect().Height();
			mItemList[i]->SetPosition(0, mItemList[i-1]->GetPosition().y - GetPosition().y + lastHeight + mSpacing);
			mItemList[i]->SetCustomWidth(maxWidth);
			mItemList[i]->SetCentered(mCenteredItems);
		}
	}
}

void TextMenu::OnLeftBtnPressed(XMFLOAT3 pos)
{
	Label* pressed = nullptr;
	for(int i = 0; i < mItemList.size(); i++)
	{
		if(mItemList[i]->PointInsideControl(pos)) {
			//if(mSoundEffect != "-none-")
			//	gSound->PlayEffect(mSoundEffect);

			// Callback.
			pressed = mItemList[i];
		}
	}

	if(pressed != nullptr && !OnItemPressed.empty())
		OnItemPressed(pressed);

}

void TextMenu::OnMouseHoover(XMFLOAT3 pos)
{
	for(int i = 0; i < mItemList.size(); i++)
	{
		if(mItemList[i]->PointInsideControl(pos))
			mItemList[i]->SetFontData(mFontData.family, mFontData.size, mHighlightColor);
		else
			mItemList[i]->SetFontData(mFontData.family, mFontData.size, mFontData.color);
	}
}

void TextMenu::SetFontData(string family, int size, UINT32 color)
{
	mFontData = FontData(family, size, color);

	for(int i = 0; i < mItemList.size(); i++)
		mItemList[i]->SetFontData(mFontData.family, mFontData.size, mFontData.color);
}

GLib::Rect TextMenu::GetRect()
{
	GLib::Rect rect(999999.0f, -999999.0f, 9999999.0f, -99999999.0f);

	for(int i = 0; i < mItemList.size(); i++)
	{
		rect.left = min(rect.left, mItemList[i]->GetRect().left);
		rect.right = max(rect.right, mItemList[i]->GetRect().right);
		rect.top = min(rect.top, mItemList[i]->GetRect().top);
		rect.bottom = max(rect.bottom, mItemList[i]->GetRect().bottom);
	}

	return rect;
}

void TextMenu::DeactivateAllItems()
{
	for(int i = 0; i < mItemList.size(); i++)
		mItemList[i]->SetFontData(mFontData.family, mFontData.size, mFontData.color);
}

void TextMenu::SetSpacing(float spacing)
{
	mSpacing = spacing;
}

void TextMenu::SetBkgdScale(float scale)
{
	mBkgdScale = scale;
}

void TextMenu::SetCenteredItems(bool centered)
{
	mCenteredItems = centered;
}

void TextMenu::SetTextColor(UINT32 defaultColor, UINT32 highlightColor)
{
	mFontData.color = defaultColor;
	mHighlightColor = highlightColor;
}

float TextMenu::GetBkgdScale()
{
	return mBkgdScale;
}

void TextMenu::ClearItems()
{
	for(int i = 0; i < mItemList.size(); i++)
		delete mItemList[i];

	mItemList.clear();
}

void TextMenu::SetPressedSound(string filename)
{
	mSoundEffect = filename;
}