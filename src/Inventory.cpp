#include "Inventory.hpp"

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <nanovg.h>

#include <iostream>

#include "Block.hpp"
#include "Math.hpp"

namespace cyberCubes {
namespace gui {

const int INV_W = 175;
const int INV_H = 165;
const int HOTBAR_CELL_SIZE = 20;
const int HOTBAR_CELL_BORDER = 1;
const int HOTBAR_W = HOTBAR_CELL_SIZE * 9 + HOTBAR_CELL_BORDER * 2;
const int HOTBAR_H = HOTBAR_CELL_SIZE + HOTBAR_CELL_BORDER * 2;
const float HOTBAR_BLOCK_SIZE = 10.0f;

float invScale = 3.0f;

Inventory::Inventory(NVGcontext &vg, render::BlockRenderer &blockRenderer) : vg(vg), blockRenderer(blockRenderer)
{
	selectedHotbarSlot = 0;
	
	inventoryImage = nvgCreateImage(&vg, "textures/gui/container/inventory.png", 0);
	widgetsImage = nvgCreateImage(&vg, "textures/gui/widgets.png", 0);
	
	hotbarCells[0].blockId = 28;
	hotbarCells[2].blockId = 30;
	hotbarCells[3].blockId = 30004;
}

Inventory::~Inventory()
{
	nvgDeleteImage(&vg, inventoryImage);
}

void Inventory::changeSelectedHotbar(int delta)
{
	selectedHotbarSlot += delta;
	if (selectedHotbarSlot > 8)
		selectedHotbarSlot %= 9;
	else if (selectedHotbarSlot < 0)
		selectedHotbarSlot += 9 * ((8 - selectedHotbarSlot) / 9);
}

void Inventory::render(int screenW, int screenH)
{
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

void Inventory::renderHotbar(int screenW, int screenH)
{
	float x = (screenW - HOTBAR_W * invScale) / 2.0f;
	float y = (screenH - HOTBAR_H * invScale) - 1;
	
	int imW, imH;
	nvgImageSize(&vg, widgetsImage, &imW, &imH);
	
	NVGpaint imgPaint = nvgImagePattern(&vg, x, y, imW * invScale, imH * invScale, 0.0f, widgetsImage, 1.0f);
	nvgBeginPath(&vg);
	nvgRect(&vg, x, y, HOTBAR_W * invScale, HOTBAR_H * invScale);
	nvgFillPaint(&vg, imgPaint);
	nvgFill(&vg);
	
	x += (selectedHotbarSlot * HOTBAR_CELL_SIZE) * invScale;
	int imdx = -1 * invScale;
	int imdy = -(HOTBAR_H + 1) * invScale;
	imgPaint = nvgImagePattern(&vg, x + imdx, y + imdy, imW * invScale, imH * invScale, 0.0f, widgetsImage, 1.0f);
	nvgBeginPath(&vg);
	nvgRect(&vg, x - 1 * invScale, y - 1 * invScale, (HOTBAR_H + 3) * invScale, (HOTBAR_H + 2) * invScale);
	nvgFillPaint(&vg, imgPaint);
	nvgFill(&vg);
}

void Inventory::renderHotbarItems(int screenW, int screenH)
{
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glDisable(GL_SCISSOR_TEST);
	glEnable(GL_DEPTH_TEST);
	
	glDepthMask(GL_TRUE);
	glClear(GL_DEPTH_BUFFER_BIT);

	
	
	for (int i = 0; i < 9; ++i)
	{
		if (hotbarCells[i].blockId >= 0)
		{
			Block *block = Block::get(hotbarCells[i].blockId);
			if (block)
			{
				math::mat4 proj = math::ortho(0.0f, (float) screenW, (float) screenH, 0.0f);
				
				float y = (screenH - HOTBAR_H * invScale / 2.0f);
				float x = (screenW - HOTBAR_W * invScale) / 2.0f + ((i + 0.5f) * HOTBAR_CELL_SIZE + HOTBAR_CELL_BORDER) * invScale;
				
				math::mat4 mvp = math::translate(proj, math::vec3(x, y, 0.0f));
				mvp = math::scale(mvp, math::vec3(HOTBAR_BLOCK_SIZE * invScale, HOTBAR_BLOCK_SIZE * invScale, 0.5f));
				mvp = math::rotate(mvp, (float) (M_PI / 2.0f), math::vec3(1.0f, 0.0f, 0.0f));

				mvp = math::rotate(mvp, (float) (3 * M_PI / 4.0f), math::vec3(0.0f, 0.0f, 1.0f));
				mvp = math::rotate(mvp, (float) (M_PI / 6.0f), math::vec3(1.0f, 1.0f, 0.0f));
				
				
	
				blockRenderer.renderInventoryBlock(block, mvp);
			}
		}
	}
}
	
}}