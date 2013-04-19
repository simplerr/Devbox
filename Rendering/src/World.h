#pragma once
#include <vector>
#include <map>
#include <boost\function.hpp>
#include <boost\bind.hpp>
#include "Utility.h"

using namespace std;

namespace GLib {
	// Forward declarations.
	class Graphics;
	class Light;
	class Sky;
	class BillboardManager;
	class Terrain;
	class Object3D;
	class WorldLoader;
	struct BillboardVertex;
	enum ObjectType;

	typedef std::vector<Object3D*> ObjectList;

	//! Contains all objects, lights and the terrain.
	class GLIB_API World
	{
	public:
		World();
		~World();

		virtual void Init(Graphics* pGraphics);
		virtual void Update(float dt);
		virtual void Draw(Graphics* pGraphics);
		void HandleCollisions();

		void AddObject(Object3D* object, int id = -1);
		void AddLight(Light* pLight);
		void RemoveObject(Object3D* pObject);
		void RemoveObject(int id);
		void RemoveLight(Light* pLight);

		LightList*	GetLights();
		ObjectList*	GetObjects();
		ObjectList	GetObjectsByType(ObjectType type);
		Terrain*	GetTerrain();
		int			GetNumLights();
		int			GetNumObjects(ObjectType type);
		XMFLOAT3	GetTerrainIntersectPoint(Ray ray);
		Object3D*	GetSelectedObject(Ray ray, ObjectType type);
		Object3D*	GetObjectById(int id);
		Object3D*	GetObjectByName(string name);
		bool		IsIntersecting(Object3D* pObjectA, Object3D* pObjectB);

		// Callback hookups.
		template <class T>
		void AddObjectAddedListener(void(T::*_callback)(Object3D*), T* _object)	{
			OnObjectAdded = boost::bind(_callback, _object, _1);
		}

		template <class T>
		void AddObjectRemovedListener(void(T::*_callback)(Object3D*), T* _object)	{
			OnObjectRemoved = boost::bind(_callback, _object, _1);
		}

		template <class T>
		void AddItemSelectedListener(void(T::*_callback)(void*, int), T* _object)	{
			OnItemSelected = boost::bind(_callback, _object, _1, _2);
		}

		template <class T>
		void AddObjectCollisionListener(void(T::*_callback)(Object3D*, Object3D*), T* _object)	{
			OnObjectCollision = boost::bind(_callback, _object, _1, _2);
		}
	private:
		// Callbacks.
		boost::function<void(Object3D*)>			OnObjectAdded;
		boost::function<void(Object3D*)>			OnObjectRemoved;
		boost::function<void(void*, int)>			OnItemSelected;
		boost::function<void(Object3D*, Object3D*)>	OnObjectCollision;
	private:
		ObjectList		mObjectList;
		LightList		mLightList;
		WorldLoader*	mWorldLoader;
		Sky*			mSkyBox;
		Terrain*		mTerrain;
		int				mIdCounter;
		int				mLightIdCounter;
	};
}