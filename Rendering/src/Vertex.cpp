#include "Vertex.h"
#include "BillboardManager.h"

//! Graphics Library namespace.
namespace GLib
{

void BillboardVertex::SetPos(XMFLOAT3 pos)
{
	Pos = pos; 
	Manager->SetRebuild(true); 
}

void BillboardVertex::SetSize(XMFLOAT2 size) 
{
	Size = size;
	Manager->SetRebuild(true); 
}

void BillboardVertex::Remove() 
{
	Manager->RemoveBillbaord(this);
}

}	// End of Graphics Library namespace.