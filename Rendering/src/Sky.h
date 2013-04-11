#pragma once

#include <string>
using namespace std;

//! Graphics Library namespace.
namespace GLib
{
	// Forward declarations.
	class Primitive;
	struct Texture2D;
	class PrimitiveFactory;
	class Graphics;

	//! A sky sphere centered around the camera.
	class Sky
	{
	public:
		Sky(Graphics* pGraphics, string texture, float radius);
		~Sky();

		void Draw(Graphics* pGraphics);
	private:
		Primitive* mPrimitive;
		Texture2D* mTexture;
	};
}