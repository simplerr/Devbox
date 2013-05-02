#include "StaticObject.h"
#include "ModelImporter.h"
#include "StaticModel.h"
#include "StaticMesh.h"
#include "Primitive.h"
#include "Graphics.h"
#include "Effects.h"

//! Graphics Library namespace.
namespace GLib
{
	//! Constructor.
	StaticObject::StaticObject(ModelImporter* importer, string filename)
		: Object3D(STATIC_OBJECT)
	{
		// Load the model.
		mModel = importer->LoadStaticModel(filename);
	}

	//! Cleanup.
	StaticObject::~StaticObject()
	{

	}

	//! Init.
	void StaticObject::Init()
	{

	}

	//! Updates the object.
	void StaticObject::Update(float dt)
	{

	}

	//! Draws the objects model.
	void StaticObject::Draw(Graphics* pGraphics)
	{
		Effects::BasicFX->SetMaterial(GetMaterial());
		mModel->Draw(pGraphics, GetWorldMatrix());

		if(IsBoundingBoxVisible())
			pGraphics->DrawBoundingBox(&GetBoundingBox(), Colors::Blue);
	}

	//! Returns true if the ray hits the mesh, triangle level check.
	bool StaticObject::RayIntersect(XMVECTOR origin, XMVECTOR direction, float& pDist)
	{
		XMMATRIX invWorld = XMMatrixInverse(&XMMatrixDeterminant(GetWorldMatrix()), GetWorldMatrix());
		origin = XMVector3TransformCoord(origin, invWorld);
		direction = XMVector3TransformNormal(direction, invWorld);
		return mModel->RayIntersect(origin, direction, pDist);
	}

	//! Returns the bounding box in world space.
	AxisAlignedBox StaticObject::GetBoundingBox()
	{
		MeshList* meshList = mModel->GetMeshList();
		AxisAlignedBox aabb = (*meshList)[0]->GetPrimitive()->GetBoundingBox();
		XMFLOAT3 min = aabb.Center - aabb.Extents;
		XMFLOAT3 max = aabb.Center + aabb.Extents;
		for(int i = 1; i < meshList->size(); i++)
		{
			AxisAlignedBox meshAABB = (*meshList)[i]->GetPrimitive()->GetBoundingBox();
			XMFLOAT3 meshMin = meshAABB.Center - meshAABB.Extents;
			XMFLOAT3 meshMax = meshAABB.Center + meshAABB.Extents;

			XMStoreFloat3(&min, XMVectorMin(XMLoadFloat3(&meshMin), XMLoadFloat3(&min)));
			XMStoreFloat3(&max, XMVectorMax(XMLoadFloat3(&meshMax), XMLoadFloat3(&max)));
		}

		aabb.Center =  (min + max) * 0.5f;
		aabb.Extents = (max - min) * 0.5f;

		// Break up the world matrix into it's components.
		XMVECTOR scale, rotation, translation;
		XMMatrixDecompose(&scale, &rotation, &translation, GetWorldMatrix());

		// Transform the AABB with the components.
		TransformAxisAlignedBoxCustom(&aabb, &aabb, scale, rotation, translation);

		return aabb;
	}

	//! Returns the model.
	StaticModel* StaticObject::GetModel()
	{
		return mModel;
	}

	//! Returns the models filename.
	string StaticObject::GetFilename()
	{
		return mModel->GetFilename();
	}
}	// End of Graphics Library namespace.