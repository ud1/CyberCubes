#include "Input.hpp"
#include "Math.hpp"

#include <SDL2/SDL.h>

namespace cyberCubes
{
namespace input
{


MainInput::MainInput(World *world, gui::Inventory *inventory, Camera *camera) : world(world), inventory(inventory), camera(camera), fovMultiplier(1.0, 0.5)
{
	mode = impl::MainInputMode::PLAYER;
	gameLoopRunning = true;
	lPressed = rPressed = false;
	flyMode = true;
	wheelY = 0;
	mouseXRel = mouseYRel = 0;
}

void MainInput::processInput(float dt)
{
	pressedKeys.clear();

	SDL_Event e;

	rClicked = rReleased = false;
	lClicked = lReleased = false;
	wheelY = 0;
	mouseXRel = mouseYRel = 0;

	while (SDL_PollEvent(&e))
	{
		if (e.type == SDL_QUIT)
			gameLoopRunning = false;
		else if (e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_ESCAPE)
			gameLoopRunning = false;

		if (e.type == SDL_KEYDOWN)
		{
			keys.insert(e.key.keysym.sym);
			pressedKeys.insert(e.key.keysym.sym);
		}

		if (e.type == SDL_KEYUP)
		{
			keys.erase(e.key.keysym.sym);
		}


		if (e.type == SDL_MOUSEMOTION)
		{
			mouseXRel += e.motion.xrel;
			mouseYRel += e.motion.yrel;
			mouseX = e.motion.x;
			mouseY = e.motion.y;
		}

		if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_RIGHT)
		{
			rClicked = true;
			rPressed = true;
		}

		if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT)
		{
			lClicked = true;
			lPressed = true;
		}

		if (e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_RIGHT)
		{
			rPressed = false;
			rReleased = true;
		}

		if (e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_LEFT)
		{
			lPressed = false;
			lReleased = true;
		}

		if (e.type == SDL_MOUSEWHEEL)
			wheelY = e.wheel.y;
	}

	if (mode == impl::MainInputMode::PAUSE)
		_processPauseMode(dt);
	else if (mode == impl::MainInputMode::PLAYER)
		_processPlayerMode(dt);
	else if (mode == impl::MainInputMode::INVENTORY)
		_processInventoryMode(dt);
}

void MainInput::_processPlayerMode(float dt)
{
	if (pressedKeys.count(SDLK_PAUSE))
	{
		mode = impl::MainInputMode::PAUSE;
		SDL_SetRelativeMouseMode(SDL_FALSE);
		return;
	}

	if (pressedKeys.count(SDLK_e))
	{
		mode = impl::MainInputMode::INVENTORY;
		SDL_SetRelativeMouseMode(SDL_FALSE);
		return;
	}

	for (int i = 0; i < 9; ++i)
	{
		if (pressedKeys.count(SDLK_1 + i))
		{
			inventory->setSelectedHotbarSlot(i);
			break;
		}
	}

	if (wheelY)
		inventory->changeSelectedHotbar(-wheelY);

	if (mouseXRel || mouseYRel)
		camera->rotate(mouseXRel, mouseYRel);

	if (pressedKeys.count(SDLK_f))
	{
		flyMode = !flyMode;
	}

	if (flyMode)
	{
		math::vec3 accel = math::vec3(0.0f, 0.0f, 0.0f);

		if (keys.count(SDLK_w))
			accel.y += 1.0;

		if (keys.count(SDLK_s))
			accel.y -= 1.0;

		if (keys.count(SDLK_d))
			accel.x += 1.0;

		if (keys.count(SDLK_a))
			accel.x -= 1.0;

		if (keys.count(SDLK_SPACE))
			accel.z += 1.0;

		if (keys.count(SDLK_LCTRL))
			accel.z -= 1.0;

		if (!keys.count(SDLK_w) && !keys.count(SDLK_s))
		{
			int sgn = (vel.y < 0.0f) ? -1 : ((vel.y > 0.0f) ? 1 : 0);
			accel.y = -sgn;
		}

		if (!keys.count(SDLK_d) && !keys.count(SDLK_a))
		{
			int sgn = (vel.x < 0.0f) ? -1 : ((vel.x > 0.0f) ? 1 : 0);
			accel.x = -sgn;
		}

		if (!keys.count(SDLK_SPACE) && !keys.count(SDLK_LCTRL))
		{
			int sgn = (vel.z < 0.0f) ? -1 : ((vel.z > 0.0f) ? 1 : 0);
			accel.z = -sgn;
		}

		accel *= 20.0f;

		math::vec3 oldVel = vel;
		vel += accel * dt;

		if (!keys.count(SDLK_w) && !keys.count(SDLK_s) && vel.y * oldVel.y < 0.0f)
			vel.y = 0.0f;

		if (!keys.count(SDLK_d) && !keys.count(SDLK_a) && vel.x * oldVel.x < 0.0f)
			vel.x = 0.0f;

		if (!keys.count(SDLK_SPACE) && !keys.count(SDLK_LCTRL) && vel.z * oldVel.z < 0.0f)
			vel.z = 0.0f;

		float maxVel = 50.0f;

		if (math::length(vel) > maxVel)
			vel *= (maxVel / math::length(vel));

		math::vec3 dPos = vel * dt;

		math::vec3 playerSize = math::vec3(0.6f, 0.6f, 1.7f);
		math::vec3 eyeOffset = math::vec3(0.3f, 0.3f, 1.6f);
		math::BBox b(camera->position - eyeOffset, playerSize);

		dPos = camera->transform(dPos.y, dPos.x, dPos.z);
		world->move(b, dPos);
		dPos = b.point - (camera->position - eyeOffset);

		math::vec3 cdPos = camera->untransform(dPos);
		cdPos = math::vec3(cdPos.y, cdPos.x, cdPos.z);
		vel = cdPos / dt;

		camera->position += dPos;
	}
	else
	{
		math::vec3 accel = math::vec3(0.0f, 0.0f, 0.0f);
		const float MAX_ACCEL = 10.0f;

		math::vec3 dirF = camera->transform(1.0, 0.0f, 0.0f);
		dirF.z = 0.0f;
		dirF = math::normalize(dirF) * MAX_ACCEL;

		math::vec3 dirR = camera->transform(0.0, 1.0f, 0.0f);
		dirR.z = 0.0f;
		dirR = math::normalize(dirR) * MAX_ACCEL;

		math::vec3 playerSize = math::vec3(0.6f, 0.6f, 1.7f);
		math::vec3 widePlayerSize = math::vec3(0.6f, 0.8f, 1.7f);
		math::vec3 eyeOffset = math::vec3(0.3f, 0.3f, 1.6f);
		bool touchGround = false, wideTouchGround = false;
		{
			math::vec3 corner = camera->position - eyeOffset;
			math::BBox b(corner, playerSize);
			math::vec3 dPos =  math::vec3(0, 0, -0.2f);
			world->move(b, dPos);

			if (b.point.z > (corner.z + dPos.z + 1e-3))
				touchGround = true;

			b.point = corner;
			b.size = widePlayerSize;

			world->move(b, dPos);

			if (b.point.z > (corner.z + dPos.z + 1e-3))
				wideTouchGround = true;
		}

		float velLen = math::length(vel);

		if (touchGround)
		{
			if (keys.count(SDLK_w))
				accel += dirF;

			if (keys.count(SDLK_s))
				accel -= dirF;

			if (keys.count(SDLK_d))
				accel += dirR;

			if (keys.count(SDLK_a))
				accel -= dirR;

			if (pressedKeys.count(SDLK_SPACE))
				vel.z = 5.0f;
		}

		if (wideTouchGround && !pressedKeys.count(SDLK_SPACE))
		{
			if (math::length(accel) > 0.01)
				vel.z += 0.2 / (1.0f + velLen / 10.0f);
		}

		vel += accel * dt / (1.0f + velLen / 10.0f);
		vel.z -= 9.8f * dt;
		float fCoef = math::pow(0.98f, dt);

		vel *= fCoef;

		if (touchGround)
		{
			math::vec3 hv = math::vec3(vel.x, vel.y, 0.0f);
			const float DV = 5.0f * dt;

			if (math::length(hv) > DV)
				hv = math::normalize(hv) * DV;

			vel -= hv;
		}

		math::vec3 dPos = vel * dt;

		math::BBox b(camera->position - eyeOffset, playerSize);
		world->move(b, dPos);
		dPos = b.point - (camera->position - eyeOffset);
		vel = dPos / dt;

		camera->position += dPos;
	}
	
	float fovMult = 1.0;
	float velLen = math::length(vel);
		
	float minVelLen = 10.0;
	if (velLen > minVelLen)
	{
		fovMult += (velLen - minVelLen) / 50.0;
	}
		
	if (fovMult > 1.3)
		fovMult = 1.3;
		
	camera->fovMultiplier = fovMultiplier.set(fovMult, dt);
}

void MainInput::_processPauseMode(float dt)
{
	if (pressedKeys.count(SDLK_PAUSE))
	{
		mode = impl::MainInputMode::PLAYER;
		SDL_SetRelativeMouseMode(SDL_TRUE);
		return;
	}
}

void MainInput::_processInventoryMode(float dt)
{
	if (pressedKeys.count(SDLK_e))
	{
		mode = impl::MainInputMode::PLAYER;
		SDL_SetRelativeMouseMode(SDL_TRUE);
		return;
	}
	

	inventory->mouse(mouseX, mouseY, lClicked);
}

}
}