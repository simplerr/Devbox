#pragma once
#include <string>
#include "d3dUtil.h"

using namespace std;

namespace GLib {
	class Graphics;

	class StatusText
	{
	public:
		StatusText();
		StatusText(string text, float x, float y, float time = 2.0f);
		~StatusText();

		void Draw(Graphics* pGraphics);
		void Update(float dt);
		void SetText(string text, float time = 2.0f, UINT32 color = 0xff000000);
		void SetSize(float size);
		void Activate(float time);
		void SetPosition(float x, float y);
		XMFLOAT3 GetPosition();
		bool IsTimeOut();
	private:
		UINT32 	 mColor;
		XMFLOAT3 mPosition;
		string	 mText;
		float	 mSize;
		float	 mTime;
	};
}