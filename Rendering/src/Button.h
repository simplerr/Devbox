#include "Label.h"
#include <boost\function.hpp>
#include <boost\bind.hpp>

namespace GLib {
	class Graphics;
	class Input;
	struct Texture2D;
}

class Button : public Label
{
public:
	Button(int x, int y, string name, string text);
	~Button();

	void Update(GLib::Input* pInput, float dt);
	void Draw(GLib::Graphics* pGraphics);
	void OnLeftBtnPressed(XMFLOAT3 pos);
	void OnMouseHoover(XMFLOAT3 pos);

	void LoadLuaProperties(LuaWrapper* pLuaWrapper);

	void SetDisabled(bool disabled);
	void SetPressedSound(string filename);
	bool GetDisabled();

	// Callback hookups.
	template <class T>
	void AddPressedListener(void(T::*_callback)(Button*), T* _object)	{
		OnPressed = boost::bind(_callback, _object, _1);
	}
private:
	boost::function<void(Button*)> OnPressed;
private:
	GLib::Texture2D* mDisabledTexture;
	GLib::Texture2D* mHooverTexture;
	bool mMouseOver;
	bool mDisabled;
	string mSoundEffect;
};