#include "Frustum.h"
#include "Vec3.h"

namespace GLib 
{
	void Frustum::ConstructFromProjection(const XMFLOAT4X4& projection)
	{
		XNA::ComputeFrustumFromProjection(this, &XMLoadFloat4x4(&projection));
	}

	bool Frustum::BoxIntersecting(const XNA::AxisAlignedBox& box) const
	{
		Vec3 boxCenter = Vec3(box.Center.x, box.Center.y, box.Center.z);
		Vec3 e = Vec3(box.Extents.x, box.Extents.y, box.Extents.z);
		float dist;

		// Do magic here

		// Optimization...
		Vec3 corner1 = boxCenter + Vec3(-e.x, -e.y, -e.z);
		Vec3 corner2 = boxCenter + Vec3(-e.x, +e.y, -e.z);
		Vec3 corner3 = boxCenter + Vec3(+e.x, -e.y, -e.z);
		Vec3 corner4 = boxCenter + Vec3(+e.x, +e.y, -e.z);
		Vec3 corner5 = boxCenter + Vec3(-e.x, -e.y, +e.z);
		Vec3 corner6 = boxCenter + Vec3(-e.x, +e.y, +e.z);
		Vec3 corner7 = boxCenter + Vec3(+e.x, -e.y, +e.z);
		Vec3 corner8 = boxCenter + Vec3(+e.x, +e.y, +e.z);

		for(int p = 0; p < 6; p++)
		{
			int insideCount = 0;

			dist = D3DXPlaneDotCoord(&mPlanes[p], &corner1);
			if(dist > 0) insideCount++;

			dist = D3DXPlaneDotCoord(&mPlanes[p], &corner2);
			if(dist > 0) insideCount++;

			dist = D3DXPlaneDotCoord(&mPlanes[p], &corner3);
			if(dist > 0) insideCount++;

			dist = D3DXPlaneDotCoord(&mPlanes[p], &corner4);
			if(dist > 0) insideCount++;

			dist = D3DXPlaneDotCoord(&mPlanes[p], &corner5);
			if(dist > 0) insideCount++;

			dist = D3DXPlaneDotCoord(&mPlanes[p], &corner6);
			if(dist > 0) insideCount++;

			dist = D3DXPlaneDotCoord(&mPlanes[p], &corner7);
			if(dist > 0) insideCount++;

			dist = D3DXPlaneDotCoord(&mPlanes[p], &corner8);
			if(dist > 0) insideCount++;

			if(insideCount == 0)
				return false;
		}


		return true;
	}

	Frustum Frustum::GetWorldFrustum(const XMFLOAT4X4& viewMatrix)
	{
		return *this;

		XMVECTOR scale, rotation, translation, detView;

		// The frustum is in view space, so we need to get the inverse view matrix
		// to transform it to world space.
		XMMATRIX invView = XMMatrixInverse(&detView, XMLoadFloat4x4(&viewMatrix));

		// Decompose the inverse view matrix and transform the frustum with the components.
		XMMatrixDecompose(&scale, &rotation, &translation, invView);
		Frustum worldFrustum = *this;
		worldFrustum.ComputePlanes();
		TransformFrustum(&worldFrustum, this, XMVectorGetX(scale), rotation, translation);

		return worldFrustum;
	}

	void Frustum::BuildFromViewProjection(XMFLOAT4X4 viewProjection)
	{
		// Left plane
		mPlanes[0].a = viewProjection._14 + viewProjection._11;
		mPlanes[0].b = viewProjection._24 + viewProjection._21;
		mPlanes[0].c = viewProjection._34 + viewProjection._31;
		mPlanes[0].d = viewProjection._44 + viewProjection._41;

		// Right plane
		mPlanes[1].a = viewProjection._14 - viewProjection._11;
		mPlanes[1].b = viewProjection._24 - viewProjection._21;
		mPlanes[1].c = viewProjection._34 - viewProjection._31;
		mPlanes[1].d = viewProjection._44 - viewProjection._41;

		// Top plane
		mPlanes[2].a = viewProjection._14 - viewProjection._12;
		mPlanes[2].b = viewProjection._24 - viewProjection._22;
		mPlanes[2].c = viewProjection._34 - viewProjection._32;
		mPlanes[2].d = viewProjection._44 - viewProjection._42;

		// Bottom plane
		mPlanes[3].a = viewProjection._14 + viewProjection._12;
		mPlanes[3].b = viewProjection._24 + viewProjection._22;
		mPlanes[3].c = viewProjection._34 + viewProjection._32;
		mPlanes[3].d = viewProjection._44 + viewProjection._42;

		// Near plane
		mPlanes[4].a = viewProjection._13;
		mPlanes[4].b = viewProjection._23;
		mPlanes[4].c = viewProjection._33;
		mPlanes[4].d = viewProjection._43;

		// Far plane
		mPlanes[5].a = viewProjection._14 - viewProjection._13;
		mPlanes[5].b = viewProjection._24 - viewProjection._23;
		mPlanes[5].c = viewProjection._34 - viewProjection._33;
		mPlanes[5].d = viewProjection._44 - viewProjection._43;

		for(int i = 0; i < 6; i++)
			D3DXPlaneNormalize(&mPlanes[i], &mPlanes[i]);
	}

	void Frustum::ComputePlanes()
	{
		/*XMVECTOR plane1, plane2, plane3, plane4, plane5, plane6;
		XNA::ComputePlanesFromFrustum(this, &plane1, &plane2, &plane3, &plane4, &plane5, &plane6);

		XMStoreFloat4(&mPlanes[0], plane1);
		XMStoreFloat4(&mPlanes[1], plane2);
		XMStoreFloat4(&mPlanes[2], plane3);
		XMStoreFloat4(&mPlanes[3], plane4);
		XMStoreFloat4(&mPlanes[4], plane5);
		XMStoreFloat4(&mPlanes[5], plane6);*/
	}
}