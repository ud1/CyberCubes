#include "models.hpp"

namespace cyberCubes
{
namespace model
{
	
ModelChest::ModelChest()
{
	model::ModelBox box;
	
	box.position = math::vec3(0.0, 7.0, 1.0f);
	box.offset = math::vec3(-7.0, -14.0, 0.0f);
	box.size = math::vec3(14.0, 14.0, 5.0f);
	box.textureOffset = math::vec2(0.0f, 0.0f);
	box.rotationFw = box.rotationUp = 0.0f;
	box.rotationR = 0.0;
	boxes.push_back(box);
	
	box.position = math::vec3(0.0, 7.0, 1.0f);
	box.offset = math::vec3(-1.0, -15.0, -2.0f);
	box.size = math::vec3(2.0, 1.0, 4.0f);
	box.textureOffset = math::vec2(0.0f, 0.0f);
	box.rotationFw = box.rotationUp = 0.0f;
	box.rotationR = 0.0;
	boxes.push_back(box);
	
	
	box.position = math::vec3(0.0, 0.0, 0.0f);
	box.offset = math::vec3(-7.0, -7.0, -8.0f);
	box.size = math::vec3(14.0, 14.0, 10.0f);
	box.textureOffset = math::vec2(0.0f, 19.0f);
	box.rotationFw = box.rotationUp = 0.0f;
	box.rotationR = 0.0;
	box.rotationUp = 0.0;
	boxes.push_back(box);
}

void ModelChest::setLidAngle(float angle)
{
	boxes[0].rotationR = boxes[1].rotationR = -angle;
}

void ModelChest::updateTextures(const ModelTextures &textures)
{
	textureOffset = textures.getTextureTile("chest").position;
}
	
}
}