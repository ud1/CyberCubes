#ifndef INVENTORY_H
#define INVENTORY_H

#include "BlockRenderer.hpp"

struct NVGcontext;

namespace cyberCubes {
namespace gui {

struct CellData
{
	int blockId = -1;
};

class Inventory
{
public:
	Inventory(NVGcontext &vg, render::BlockRenderer &blockRenderer);
	~Inventory();
	
	void render(int screenW, int screenH);
	void renderHotbar(int screenW, int screenH);
	void renderHotbarItems(int screenW, int screenH);

	void changeSelectedHotbar(int delta);
	
	const CellData &getHotbarCell(int cell) const
	{
		return hotbarCells[cell];
	}
	
	int getSelectedHotbarSlot() const
	{
		return selectedHotbarSlot;
	}
	
	void setSelectedHotbarSlot(int slot)
	{
		selectedHotbarSlot = slot;
	}
private:
	NVGcontext &vg;
	int inventoryImage;
	int widgetsImage;
	
	int selectedHotbarSlot;
	
	CellData hotbarCells[9];
	
	render::BlockRenderer &blockRenderer;
};

}}

#endif // INVENTORY_H