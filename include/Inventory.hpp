#ifndef INVENTORY_H
#define INVENTORY_H

#include "BlockRenderer.hpp"
#include <vector>

struct NVGcontext;

namespace cyberCubes {
namespace gui {

struct CellData
{
	int blockId = -1;
	int count = 0;
};

struct Slot
{
	int x, y, size;
	CellData cellData;
	bool canPut, canGet;
};

class Inventory
{
public:
	Inventory(NVGcontext &vg, render::BlockRenderer &blockRenderer);
	~Inventory();
	
	void setWindowSize(int screenW, int screenH)
	{
		this->screenW = screenW;
		this->screenH = screenH;
	}
	
	void refreshNEIItems();
	void render();
	void renderInventoryItems();
	void renderHotbar();
	void renderHotbarItems();
	void renderNEI();
	void renderNEIItems();
	void renderDraggedItem();

	void changeSelectedHotbar(int delta);
	
	const CellData &getHotbarCell(int cell) const;
	
	int getSelectedHotbarSlot() const
	{
		return selectedHotbarSlot;
	}
	
	void setSelectedHotbarSlot(int slot)
	{
		selectedHotbarSlot = slot;
	}
	
	void mouse(int x, int y, bool lClicked);
private:
	std::vector<Slot> slots;
	Slot draggedCellData;
	std::vector<const Block *> _NEIItems;
	
	NVGcontext &vg;
	int inventoryImage;
	int widgetsImage;
	int screenW, screenH;
	
	int selectedHotbarSlot;
	
	render::BlockRenderer &blockRenderer;
	
	void _render(const Block *block, float x, float y);
};

}}

#endif // INVENTORY_H