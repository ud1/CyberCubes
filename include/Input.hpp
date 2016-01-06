#ifndef INPUT_H
#define INPUT_H

#include <set>

#include "Inventory.hpp"
#include "Camera.hpp"
#include "World.hpp"

namespace cyberCubes {
namespace input {

namespace impl {
	enum class MainInputMode
	{
		PLAYER,
		INVENTORY,
		PAUSE
	};
}

class MainInput
{
public:
	MainInput(World *world, gui::Inventory *inventory, Camera *camera);
	void processInput(float dt);
	
	bool isInventoryMode() const
	{
		return mode == impl::MainInputMode::INVENTORY;
	}
	
	bool isPlayerMode() const
	{
		return mode == impl::MainInputMode::PLAYER;
	}

	impl::MainInputMode mode;
	bool rClicked, rReleased;
	bool lClicked, lReleased;
	bool gameLoopRunning;
	std::set<int> keys, pressedKeys;
	bool lPressed;
	bool rPressed;
	bool flyMode;
	int wheelY;
	int mouseXRel, mouseYRel;
	int mouseX, mouseY;
	math::vec3 vel;
	FloatSmoothing fovMultiplier;
	
	gui::Inventory *inventory;
	Camera *camera;
	World *world;
	
	void _processPlayerMode(float dt);
	void _processPauseMode(float dt);
	void _processInventoryMode(float dt);
};



}}
#endif // INPUT_H
