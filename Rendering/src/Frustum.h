#pragma once

#include "Utility.h"

namespace GLib {
	class GLIB_API Frustum : public XNA::Frustum
	{
	public:
		void BuildFromViewProjection(XMFLOAT4X4 viewProjection);

		void ConstructFromProjection(const XMFLOAT4X4& projection);

		bool BoxIntersecting(const XNA::AxisAlignedBox& box) const;
		Frustum GetWorldFrustum(const XMFLOAT4X4& viewMatrix);

		void ComputePlanes();
	private:
		D3DXPLANE mPlanes[6];// Planes
	};
}