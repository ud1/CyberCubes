#include "Inventory.hpp"

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <nanovg.h>

namespace cyberCubes {
namespace gui {

const int INV_W = 175;
const int INV_H = 165;

Inventory::Inventory(NVGcontext &vg) : vg(vg)
{
	inventoryImage = nvgCreateImage(&vg, "textures/gui/container/inventory.png", 0);
}

Inventory::~Inventory()
{
	nvgDeleteImage(&vg, inventoryImage);
}

void Inventory::render(int screenW, int screenH)
{
	float invScale = 2.0f;
	
	float x = (screenW - INV_W * invScale) / 2.0f;
	float y = (screenH - INV_H * invScale) / 2.0f;
	
	int imW, imH;
	nvgImageSize(&vg, inventoryImage, &imW, &imH);
	
	NVGpaint imgPaint = nvgImagePattern(&vg, x, y, imW * invScale, imH * invScale, 0.0f, inventoryImage, 1.0f);
	nvgBeginPath(&vg);
	nvgRect(&vg, x, y, INV_W * invScale, INV_H * invScale);
	nvgFillPaint(&vg, imgPaint);
	nvgFill(&vg);
}
	
}}