#include "Inventory.hpp"

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <nanovg.h>

#include <iostream>
#include <algorithm>

#include "Block.hpp"
#include "Math.hpp"

namespace cyberCubes {
namespace gui {

const int INV_W = 175;
const int INV_H = 166;
const float CELL_SIZE = 16;
const int CELL_DIST = 18; 
const int HOTBAR_CELL_SIZE = 20;
const int HOTBAR_CELL_BORDER = 1;
const int HOTBAR_W = HOTBAR_CELL_SIZE * 9 + HOTBAR_CELL_BORDER * 2;
const int HOTBAR_H = HOTBAR_CELL_SIZE + HOTBAR_CELL_BORDER * 2;
const float HOTBAR_BLOCK_SIZE = 10.0f;
const int HOTBAR_CELL_INDEX = 35;

float invScale = 3.0f;

Inventory::Inventory(NVGcontext &vg, render::BlockRenderer &blockRenderer) : vg(vg), blockRenderer(blockRenderer)
{
	selectedHotbarSlot = 0;
	
	inventoryImage = nvgCreateImage(&vg, "textures/gui/container/inventory.png", 0);
	widgetsImage = nvgCreateImage(&vg, "textures/gui/widgets.png", 0);
	
	slots.reserve(45);
	
	for (int i = 0; i < 4; ++i)
	{
		Slot slot;
		slot.x = 8;
		slot.y = 8 + i * CELL_DIST;
		slot.size = 15;
		slot.canGet = true;
		slot.canPut = true;
		slots.push_back(slot);
	}
	
	for (int i = 0; i < 4; ++i)
	{
		Slot slot;
		slot.x = 88 + (i % 2) * CELL_DIST;
		slot.y = 26 + (i / 2) * CELL_DIST;
		slot.size = 15;
		slot.canGet = true;
		slot.canPut = true;
		slots.push_back(slot);
	}
	
	{
		Slot slot;
		slot.x = 144;
		slot.y = 36;
		slot.size = 15;
		slot.canGet = true;
		slot.canPut = false;
	}
	
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 9; ++j)
		{
			Slot slot;
			slot.x = 8 + j * CELL_DIST;
			
			if (i < 3)
			{
				slot.y = 84 + i * CELL_DIST;
			}
			else
			{
				slot.y = 142;
			}
			
			slot.cellData.blockId = i * 9 + j;
			
			slot.size = 15;
			slot.canGet = true;
			slot.canPut = true;
			slots.push_back(slot);
		}
	}
	
	draggedCellData.cellData.blockId = -1;
}

const CellData &Inventory::getHotbarCell(int cell) const
{
	return slots[cell + HOTBAR_CELL_INDEX].cellData;
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

math::vec2 getInventoryPosition(int screenW, int screenH)
{
	float x = floor((screenW - INV_W * invScale) / 2.0f);
	float y = floor((screenH - INV_H * invScale) / 2.0f);
	
	return math::vec2(x, y);
}

void Inventory::render()
{
	math::vec2 pos = getInventoryPosition(screenW, screenH);
	
	int imW, imH;
	nvgImageSize(&vg, inventoryImage, &imW, &imH);
	
	NVGpaint imgPaint = nvgImagePattern(&vg, pos.x, pos.y, imW * invScale, imH * invScale, 0.0f, inventoryImage, 1.0f);
	nvgBeginPath(&vg);
	nvgRect(&vg, pos.x, pos.y, INV_W * invScale, INV_H * invScale);
	nvgFillPaint(&vg, imgPaint);
	nvgFill(&vg);
}

void Inventory::_render(const Block *block, float x, float y)
{
	if (block)
	{
		math::mat4 proj = math::ortho(0.0f, (float) screenW, (float) screenH, 0.0f);
		
		math::mat4 mvp = math::translate(proj, math::vec3(x, y, 0.0f));
		mvp = math::scale(mvp, math::vec3(HOTBAR_BLOCK_SIZE * invScale, HOTBAR_BLOCK_SIZE * invScale, 0.5f));
		mvp = math::rotate(mvp, (float) (M_PI / 2.0f), math::vec3(1.0f, 0.0f, 0.0f));
		mvp = math::rotate(mvp, (float) (3 * M_PI / 4.0f), math::vec3(0.0f, 0.0f, 1.0f));
		mvp = math::rotate(mvp, (float) (M_PI / 6.0f), math::vec3(1.0f, 1.0f, 0.0f));
			
		blockRenderer.renderInventoryBlock(block, mvp);
	}
}

void Inventory::renderInventoryItems()
{
	math::vec2 pos = getInventoryPosition(screenW, screenH);
	
	for (const Slot &slot : slots)
	{
		if (slot.cellData.blockId >= 0)
		{
			Block *block = Block::get(slot.cellData.blockId);
			
			float y = pos.y + (slot.y + 0.5 * CELL_SIZE) * invScale;
			float x = pos.x + (slot.x + 0.5 * CELL_SIZE) * invScale;
		
			_render(block, x, y);
		}
	}
}

void getNEICoords(int screenW, int screenH, math::vec2 &p1, math::vec2 &p2)
{
	float margin = 20.0f;
	p1 = math::vec2(floor((screenW + INV_W * invScale) / 2.0f + margin), margin);
	p2 = math::vec2(screenW - margin, screenH - margin);
}

void Inventory::renderNEI()
{
	math::vec2 p1, p2;
	getNEICoords(screenW, screenH, p1, p2);
	
	nvgBeginPath(&vg);
	nvgRect(&vg, p1.x, p1.y, p2.x - p1.x, p2.y - p1.y);
	nvgFillColor(&vg, nvgRGBAf(0.6, 0.6, 0.6, 0.9));
	nvgFill(&vg);
}

void Inventory::refreshNEIItems()
{
	_NEIItems.clear();
	const std::unordered_map<int, Block> &blocks = Block::getBlockIdMap();
	
	for (std::unordered_map<int, Block>::const_iterator it = blocks.begin(); it != blocks.end(); ++it)
	{
		_NEIItems.push_back(&it->second);
	}
	
	std::sort(_NEIItems.begin(), _NEIItems.end(), [](const Block *b1, const Block *b2) -> bool {return b1->id < b2->id;});
}

math::vec2 _getNEIItemPosition(int screenW, int screenH, int i)
{
	float margin = 5.0f;
	math::vec2 p1, p2;
	getNEICoords(screenW, screenH, p1, p2);
	
	int rowSize = floor((p2.x - p1.x - 2.0*margin) / (CELL_DIST * invScale));
	if (rowSize < 1)
		rowSize = 1;
	
	float left = (p2.x - p1.x - rowSize * (CELL_DIST * invScale)) / 2.0;
	
	float x = p1.x + (i % rowSize + 0.5f) * CELL_DIST * invScale + left;
	float y = p1.y + (i / rowSize + 0.5f) * CELL_DIST * invScale + margin;
	return math::vec2(x, y);
}

void Inventory::renderNEIItems()
{
	int i = 0;
	for (const Block *block : _NEIItems)
	{
		math::vec2 pos = _getNEIItemPosition(screenW, screenH, i);
		_render(block, pos.x, pos.y);
		++i;
	}
}

void Inventory::renderDraggedItem()
{
	if (draggedCellData.cellData.blockId >= 0)
	{
		glClear(GL_DEPTH_BUFFER_BIT);
		Block *block = Block::get(draggedCellData.cellData.blockId);
		_render(block, draggedCellData.x, draggedCellData.y);
	}
}

void Inventory::mouse(int x, int y, bool lClicked)
{
	math::vec2 pos = getInventoryPosition(screenW, screenH);
	math::vec2 relPos = (math::vec2((float) x, (float) y) - pos) / invScale;
	
	draggedCellData.x = x;
	draggedCellData.y = y;
	
	if (lClicked)
	{
		for (Slot &slot : slots)
		{
			if (slot.x < relPos.x && slot.y < relPos.y && relPos.x < (slot.x + slot.size) && relPos.y < (slot.y + slot.size))
			{
				std::swap(slot.cellData, draggedCellData.cellData);
				break;
			}
		}
		
		if (!(pos.x <= x && x <= (pos.x + INV_W * invScale) && pos.y <= y && y <= (pos.y + INV_H * invScale)))
		{
			draggedCellData.cellData.blockId = -1;
		}
		
		float halfCellSize = 0.5 * CELL_SIZE * invScale;
		int i = 0;
		for (const Block *block : _NEIItems)
		{
			math::vec2 pos = _getNEIItemPosition(screenW, screenH, i);
			if ((pos.x - halfCellSize) < x && (pos.y - halfCellSize) < y && (x < (pos.x + halfCellSize) && y < (pos.y + halfCellSize)))
			{
				draggedCellData.cellData.blockId = block->id;
				draggedCellData.cellData.count = 1;
				break;
			}
			++i;
		}
	}
}

void Inventory::renderHotbar()
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

void Inventory::renderHotbarItems()
{
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glDisable(GL_SCISSOR_TEST);
	glEnable(GL_DEPTH_TEST);
	
	glDepthMask(GL_TRUE);
	glClear(GL_DEPTH_BUFFER_BIT);
	
	for (int i = 0; i < 9; ++i)
	{
		if (slots[i + HOTBAR_CELL_INDEX].cellData.blockId >= 0)
		{
			Block *block = Block::get(slots[i + HOTBAR_CELL_INDEX].cellData.blockId);
			
			float y = (screenH - HOTBAR_H * invScale / 2.0f);
			float x = (screenW - HOTBAR_W * invScale) / 2.0f + ((i + 0.5f) * HOTBAR_CELL_SIZE + HOTBAR_CELL_BORDER) * invScale;
			_render(block, x, y);
		}
	}
}
	
}}