#pragma once

#include <string>
using namespace std;

namespace GLib {
	// Forward declarations.
	class Terrain;
	class World;
	class ModelLoaderXML;

	//! Loads and saves the world to a file.
	class WorldLoader
	{
	public:
		WorldLoader();
		~WorldLoader();

		void LoadWorld(World* pWorld, Terrain* pTerrain, string filename);
		void SaveWorld(World* pWorld, Terrain* pTerrain, string filename);
	};
}