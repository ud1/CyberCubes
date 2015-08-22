#ifndef INVENTORY_H
#define INVENTORY_H

struct NVGcontext;

namespace cyberCubes {
namespace gui {
	
class Inventory
{
public:
	Inventory(NVGcontext &vg);
	~Inventory();
	
	void render(int screenW, int screenH);

private:
	NVGcontext &vg;
	int inventoryImage;
};

}}

#endif // INVENTORY_H