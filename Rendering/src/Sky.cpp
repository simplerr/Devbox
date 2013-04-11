#include "Sky.h"
#include "Graphics.h"
#include "Runnable.h"
#include "PrimitiveFactory.h"
#include "Camera.h"
#include "Effects.h"
#include "Primitive.h"
#include "PrimitiveFactory.h"

//! Graphics Library namespace.
namespace GLib
{

//! Constructor.
Sky::Sky(Graphics* pGraphics, string texture, float radius)
{
	mTexture = pGraphics->LoadTexture(texture);
	mPrimitive = pGraphics->GetPrimitiveFactory()->CreateSphere(radius, 30, 30);
}
	
//! Cleanup.
Sky::~Sky()
{

}

void Sky::Draw(Graphics* pGraphics)
{
	// Center the the skybox around the camera.
	Camera* camera = pGraphics->GetCamera();
	XMFLOAT3 eyePos = camera->GetPosition();
	XMMATRIX T = XMMatrixTranslation(eyePos.x, eyePos.y, eyePos.z);

	XMMATRIX WVP = T * XMLoadFloat4x4(&camera->GetViewMatrix()) * XMLoadFloat4x4(&camera->GetProjectionMatrix());

	Effects::SkyFX->SetWorldViewProj(WVP);
	Effects::SkyFX->SetCubeMap(mTexture->shaderResourceView);

	ID3D11DeviceContext* context = pGraphics->GetContext();
	context->IASetInputLayout(Effects::SkyFX->GetInputLayout());
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	Effects::SkyFX->GetTech()->GetPassByIndex(0)->Apply(0, context);
	mPrimitive->Draw<Vertex>(context);

	mPrimitive->Draw<Vertex>(context); // Why? [NOTE][TODO]
}

}	// End of Graphics Library namespace.