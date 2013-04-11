#pragma once

#include <map>
#include <string>
#include <D3DX11.h>
#include <xnamath.h>
#include <vector>
#include "Vertex.h"
using namespace std;

//! Graphics Library namespace.
namespace GLib
{
	// Forward declarations.
	class Primitive;
	class Terrain;

	//!
	//	Handles creation of different primitives.
	//	Stores each primitive in a map to enable instancing.
	//!
	class PrimitiveFactory
	{
	public:
		PrimitiveFactory();
		~PrimitiveFactory();

		void AddPrimitive(string name, Primitive* primitive);

		Primitive* CreateBox();
		Primitive* CreateGrid(float width, float depth, UINT m, UINT n);
		Primitive* CreateQuad();
		Primitive* CreateSphere(float radius, UINT sliceCount, UINT stackCount);
		Primitive* CreateTerrain(Terrain* terrain);
	private:
		map<string, Primitive*> mPrimitiveMap;
	};
}