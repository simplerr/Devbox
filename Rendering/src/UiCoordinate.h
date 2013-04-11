#pragma once
#include "d3dUtil.h"
#include "Graphics.h"
#include "D3DCore.h"

enum UiAlignmentX
{
	CENTER,
	LEFT,
	RIGHT,
};

enum UiAlignmentY
{
	CENTER_Y,
	TOP,
	BOTTOM,
};

class UiCoordinate
{
public:
	UiCoordinate(UiAlignmentX alignmentX, UiAlignmentY alignmentY, float x, float y, float width, float height, bool scaleHeight, bool scaleWidth) {

		GLib::Rect ratio = GLib::GlobalApp::GetGraphics()->GetD3D()->GetDimensionRatio();

		//	Width
		if(scaleWidth)
			this->width = width / ratio.right;
		else
			this->width = width;

		// Height
		if(scaleHeight)
			this->height = height / ratio.bottom;
		else
			this->height = height;

		//	X position
		if(alignmentX == CENTER)
			this->x = x / ratio.right;
		else if(alignmentX == LEFT) 
			this->x = ((x - width/2) / ratio.bottom) + width/2;
		else if(alignmentX == RIGHT) 
			this->x = ((x + width/2) / ratio.bottom) - width/2;

		this->y = y / ratio.bottom;

		//	Y position
		if(alignmentY == CENTER)
			this->y = y / ratio.bottom;
		else if(alignmentY == TOP) 
			this->y = ((y - height/2) / ratio.bottom) + height/2;
		else if(alignmentY == BOTTOM) 
			this->y = ((y + height/2) / ratio.bottom) - height/2;
	}

	float x;
	float y;
	float width;
	float height;
};