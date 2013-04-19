#include "WorldLoader.h"
#include "World.h"
#include "Terrain.h"
#include "Graphics.h"
#include "tinyxml/tinystr.h";
#include "tinyxml/tinyxml.h"
#include <fstream>
#include "Object3D.h"
#include "LightObject.h"
#include "Light.h"
#include "StaticObject.h"
#include "AnimatedObject.h"

//! Graphics Library namespace.
namespace GLib
{
	//! Constructor.
	WorldLoader::WorldLoader()
	{

	}

	//! Cleanup.
	WorldLoader::~WorldLoader()
	{

	}

	//! Load objects, blend and height data from a file.
	void WorldLoader::LoadWorld(World* pWorld, Terrain* pTerrain, string filename)
	{
		ifstream fin(filename);

		//
		// Load the terrain init data.
		//
		InitInfo data;
		string ignore;
		fin >> ignore;	// -TERRAIN_DATA-
		fin >> ignore >> data.HeightMap;
		fin >> ignore >> data.BlendMap;
		fin >> ignore >> data.LayerMap0;
		fin >> ignore >> data.LayerMap1;
		fin >> ignore >> data.LayerMap2;
		fin >> ignore >> data.LayerMap3;
		fin >> ignore >> data.LayerMap4;
		fin >> ignore >> data.HeightmapWidth;
		fin >> ignore >> data.HeightmapHeight;
		fin >> ignore >> data.HeightScale;
		fin >> ignore >> data.CellSpacing;
		pTerrain->Init(GlobalApp::GetD3DDevice(), GlobalApp::GetD3DContext(), GlobalApp::GetGraphics()->GetPrimitiveFactory(), data);

		//
		// Load all objects.
		//
		int objects, type;
		string name, fileName, trash;
		XMFLOAT3 pos, rot, sca;
		fin >> ignore >> ignore >> objects;
		for(int i = 0; i < objects; i++)
		{
			Object3D* object = nullptr;

			fin >> trash >> type;	// Object type
			fin >> trash >> name;	// Name
			fin >> trash >> pos.x >> trash >> pos.y >>  trash >> pos.z;	// Position
			fin >> trash >> rot.x >> trash >> rot.y >> trash >> rot.z;	// Rotation
			fin >> trash >> sca.x >> trash >> sca.y >> trash >> sca.z;	// Scale
			fin >> trash >> fileName;

			if(type == STATIC_OBJECT)		// Static object
				object = new StaticObject(GlobalApp::GetGraphics()->GetModelImporter(), fileName);
			else if(type == ANIMATED_OBJECT) // Animated object
				object = new AnimatedObject(GlobalApp::GetGraphics()->GetModelImporter(), fileName);
			else if(type == LIGHT_OBJECT)	// LightObject
			{ 
				int lightType;
				float range, spot;
				XMFLOAT4 ambient, diffuse, specular;
				XMFLOAT3 intensity, att;

				fin >> trash >> lightType;	// Light type
				fin >> trash >> ambient.x >> trash >> ambient.y >> trash >> ambient.z >> trash >> ambient.w;		// Ambient
				fin >> trash >> diffuse.x >> trash >> diffuse.y >> trash >> diffuse.z >> trash >> diffuse.w;		// Diffuse
				fin >> trash >> specular.x >> trash >> specular.y >> trash >> specular.z >> trash >> specular.w;	// Specular
				fin >> trash >> intensity.x >> trash >> intensity.y >> trash >> intensity.z;	// Intensity
				fin >> trash >> att.x >> trash >> att.y >> trash >> att.z;	// Att
				fin >> trash >> range;					// Range
				fin >> trash >> spot;					// Spot

				object = new LightObject();
				object->SetMaterials(Material(ambient, diffuse, specular));
				LightObject* lightObject = (LightObject*)object;
				lightObject->SetLightType((LightType)lightType);
				lightObject->SetIntensity(intensity.x, intensity.y, intensity.z);
				lightObject->SetAtt(att.x, att.y, att.z);
				lightObject->SetRange(range);
				lightObject->SetSpot(spot);
			}

			// Common for all objects.
			object->SetName(name);
			object->SetPosition(pos);
			object->SetRotation(rot);
			object->SetScale(sca);

			pWorld->AddObject(object);
		}

		//
		// Load the height and blend maps.
		//
		pTerrain->LoadHeightmap(fin);
		pTerrain->LoadBlendMap(fin);

		fin.close();
	}

	//! Saves objects, blend and height data to a file.
	void WorldLoader::SaveWorld(World* pWorld, Terrain* pTerrain, string filename)
	{
		ofstream fout(filename);

		// Save the terrain data.
		InitInfo data = pTerrain->GetInfo();
		fout << "----------------------------TERRAIN_DATA----------------------------" << endl;
		fout << "HeightMap " << data.HeightMap << endl;
		fout << "BlendMap " << data.BlendMap << endl;
		fout << "LayerMap0 " << data.LayerMap0 << endl;
		fout << "LayerMap1 " << data.LayerMap1 << endl;
		fout << "LayerMap2 " << data.LayerMap2 << endl;
		fout << "LayerMap3 " << data.LayerMap3 << endl;
		fout << "LayerMap4 " << data.LayerMap4 << endl;
		fout << "HeightMapWidth " << data.HeightmapHeight << endl;
		fout << "HeightMapHeight " << data.HeightmapHeight << endl;
		fout << "HeightScale " << data.HeightScale << endl;
		fout << "CellSpacing " << data.CellSpacing << endl;

		ObjectList* objects = pWorld->GetObjects();
		fout << "\n----------------------------OBJECTS----------------------------" << endl;
		fout << "Objects " << objects->size() << endl;
		for(int i = 0; i < objects->size(); i++)
		{
			Object3D* object = objects->operator[](i);

			// Common for all objects.
			XMFLOAT3 pos = object->GetPosition();
			XMFLOAT3 rot = object->GetRotation();
			XMFLOAT3 sca = object->GetScale();
			fout << object->GetType() << " " << object->GetName() << " ";	// Type & Name
			fout << pos.x << " " << pos.y << " " << pos.z << " ";	// Position
			fout << rot.x << " " << rot.y << " " << rot.z << " ";	// Rotation
			fout << sca.x << " " << sca.y << " " << sca.z << " ";	// Scale
			fout << object->GetFilename() << " ";
		
			// Save light data.
			if(object->GetType() == LIGHT_OBJECT) 
			{
				LightObject* light = (LightObject*)object;

				Material mat = light->GetMaterial();
				XMFLOAT3 intensity = light->GetIntensity();
				XMFLOAT3 att = light->GetAtt();
				fout << light->GetLightType() << " ";	// Light type.
				fout << mat.ambient.x << " " << mat.ambient.y << " " << mat.ambient.z << " " << mat.ambient.w << " ";		// Ambient
				fout << mat.diffuse.x << " " << mat.diffuse.y << " " << mat.diffuse.z << " " << mat.diffuse.w << " ";		// Diffuse
				fout << mat.specular.x << " " << mat.specular.y << " " << mat.specular.z << " " << mat.specular.w << " ";	// Specular
				fout << intensity.x << " " << intensity.y << " " << intensity.z << " ";	// Intensity
				fout << att.x << " " << att.y << " " << att.z << " ";	// Att
				fout << light->GetRange() << " ";	// Range
				fout << light->GetSpot() << " ";	// Spot
			}

			fout << endl;
		}

		pTerrain->SaveHeightMap(fout);
		pTerrain->SaveBlendMap(fout);

		fout.close();
	}
}	// End of Graphics Library namespace.