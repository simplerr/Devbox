#include "Label.h"
#include "Graphics.h"
#include "d3dUtil.h"
#include "LuaWrapper.h"
#include "UiCoordinate.h"

GLib::Texture2D* texture;

Label::Label(int x, int y, string name, string text)
	: BasicControl(x, y, name)
{
	SetFontData("Arial", 30);
	SetText(text);
	SetBkgdTexture("data/textures/crate.dds");
	SetCustomWidth(0);
}

Label::~Label()
{

}

void Label::Draw(GLib::Graphics* pGraphics)
{
	if(IsDrawingBkgd()) 
	{
		GLib::Rect rect = GetRect();
		pGraphics->DrawScreenQuad(GetBkgdTexture(), GetPosition().x, GetPosition().y, rect.Width(), rect.Height());
	}

	if(mCentered) {
		pGraphics->DrawText(GetText(), GetPosition().x - GetTextWidth()/2, GetPosition().y - GetTextHeight()/2 - mFontData.size/5.0f, mFontData.size, mFontData.color, mFontData.family);
	}
	else
		pGraphics->DrawText(GetText(), GetPosition().x - GetTextWidth()/2, GetPosition().y - GetTextHeight()/2 - mFontData.size/5.0f, mFontData.size, mFontData.color, mFontData.family);
}

void Label::LoadLuaProperties(LuaWrapper* pLuaWrapper)
{
	float x = pLuaWrapper->Get<float>(GetName() + ".pos_x");
	float y = pLuaWrapper->Get<float>(GetName() + ".pos_y");
	float fontSize = pLuaWrapper->Get<float>(GetName() + ".font_size");
	float bkgdScale = pLuaWrapper->Get<float>(GetName() + ".bkgd_scale");
	string fontFamily = pLuaWrapper->Get<string>(GetName() + ".font_family");
	string color = pLuaWrapper->Get<string>(GetName() + ".font_color");

	SetPosition(x, y);
	SetFontData(fontFamily, fontSize, GLib::StripRGBA(color));
	SetBkgdScale(bkgdScale);
}

void Label::OnLeftBtnPressed(XMFLOAT3 pos)
{

}

void Label::OnMouseHoover(XMFLOAT3 pos)
{

}

//! Returns the labels rect in global coordinates.
GLib::Rect Label::GetRect()
{
	GLib::Rect rect = GLib::GlobalApp::GetGraphics()->MeasureText(GetText(), mFontData.size, mFontData.family);

	// Scale the rect in with GetBkgdScale()
	float x = rect.left + rect.Width()/2;
	float y = rect.top + rect.Height()/2;

	float width = rect.Width() * GetBkgdScale();
	float height = rect.Height() * GetBkgdScale();

	rect.left = x - width;
	rect.right = x;
	rect.top = y - height / 2;
	rect.bottom = y + height / 2;

	if(mCustomWidth == 0)
		return GLib::Rect(GetPosition().x-width/2, rect.right + GetPosition().x, GetPosition().y - height/2, rect.bottom + GetPosition().y - height/2);
	else
		return GLib::Rect(GetPosition().x-width/2, mCustomWidth + GetPosition().x - width/2, GetPosition().y - height/2, rect.bottom + GetPosition().y - height/2);
}

float Label::GetTextWidth()
{
	return GLib::GlobalApp::GetGraphics()->MeasureText(GetText(), mFontData.size, mFontData.family).Width();
}

float Label::GetTextHeight()
{
	return GLib::GlobalApp::GetGraphics()->MeasureText(GetText(), mFontData.size, mFontData.family).Height();
}

void Label::SetFontData(string family, int size, UINT32 color)
{
	mFontData.family = family;
	mFontData.size = size;
	mFontData.color = color;
}

void Label::SetText(string text)
{
	mText = text;
}

string Label::GetText()
{
	return mText;
}

void Label::SetCustomWidth(int width)
{
	mCustomWidth = width;
}

void Label::SetCentered(bool centered)
{
	mCentered = centered;
}