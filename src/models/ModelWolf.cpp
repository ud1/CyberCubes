#include "models.hpp"

namespace cyberCubes
{
namespace model
{

ModelWolf::ModelWolf()
{
	model::ModelBox box;
	
	box.position = math::vec3(-1.0, -3.0, 2.0f);
	box.offset = math::vec3(-4.0, -3.0, 3.0f);
	box.size = math::vec3(8.0, 7.0, -6.0f);
	box.textureOffset = math::vec2(21.0f, 0.0f);
	box.rotationFw = box.rotationUp = 0.0f;
	box.rotationR = M_PI / 2.0;
	boxes.push_back(box);
	
	box.position = math::vec3(0.0, 2.0, 2.0f);
	box.offset = math::vec3(-4.0, -3.0, 2.0f);
	box.size = math::vec3(6.0, 6.0, -9.0f);
	box.textureOffset = math::vec2(18.0f, 14.0f);
	box.rotationFw = box.rotationUp = 0.0f;
	box.rotationR = M_PI / 2.0;
	boxes.push_back(box);
	
	box.position = math::vec3(-1.0, -7.0, 2.5f);
	box.offset = math::vec3(-3.0, -2.0, 3.0f);
	box.size = math::vec3(6.0, 4.0, -6.0f);
	box.textureOffset = math::vec2(0.0f, 0.0f);
	box.rotationFw = box.rotationUp = 0.0f;
	box.rotationR = 0.0;
	boxes.push_back(box);
	
	box.position = math::vec3(-0.5f, -7.0, 2.5f);
	box.offset = math::vec3(-2.0f, -5.0f, 0.0f);
	box.size = math::vec3(3.0, 4.0, -3.0f);
	box.textureOffset = math::vec2(0.0f, 10.0f);
	box.rotationFw = box.rotationUp = 0.0f;
	box.rotationR = 0.0;
	boxes.push_back(box);
	
	box.position = math::vec3(-1.0f, -7.0, 2.5f);
	box.offset = math::vec3(1.0f, 0.0f, 5.0f);
	box.size = math::vec3(2.0, 1.0, -2.0f);
	box.textureOffset = math::vec2(16.0f, 14.0f);
	box.rotationFw = box.rotationUp = 0.0f;
	box.rotationR = 0.0;
	boxes.push_back(box);
	
	box.position = math::vec3(-1.0f, -7.0, 2.5f);
	box.offset = math::vec3(-3.0f, 0.0f, 5.0f);
	box.size = math::vec3(2.0, 1.0, -2.0f);
	box.textureOffset = math::vec2(16.0f, 14.0f);
	box.rotationFw = box.rotationUp = 0.0f;
	box.rotationR = 0.0;
	boxes.push_back(box);
	
	box.position = math::vec3(0.5f, -4.0, 0.0f);
	box.offset = math::vec3(-1.0f, -1.0f, 0.0f);
	box.size = math::vec3(2.0, 2.0, -8.0f);
	box.textureOffset = math::vec2(0.0f, 18.0f);
	box.rotationFw = box.rotationUp = 0.0f;
	box.rotationR = 0.0;
	boxes.push_back(box);
	
	box.position = math::vec3(-2.5f, -4.0, 0.0f);
	box.offset = math::vec3(-1.0f, -1.0f, 0.0f);
	box.size = math::vec3(2.0, 2.0, -8.0f);
	box.textureOffset = math::vec2(0.0f, 18.0f);
	box.rotationFw = box.rotationUp = 0.0f;
	box.rotationR = 0.0;
	boxes.push_back(box);
	
	box.position = math::vec3(0.5f, 7.0, 0.0f);
	box.offset = math::vec3(-1.0f, -1.0f, 0.0f);
	box.size = math::vec3(2.0, 2.0, -8.0f);
	box.textureOffset = math::vec2(0.0f, 18.0f);
	box.rotationFw = box.rotationUp = 0.0f;
	box.rotationR = 0.0;
	boxes.push_back(box);
	
	box.position = math::vec3(-2.5f, 7.0, 0.0f);
	box.offset = math::vec3(-1.0f, -1.0f, 0.0f);
	box.size = math::vec3(2.0, 2.0, -8.0f);
	box.textureOffset = math::vec2(0.0f, 18.0f);
	box.rotationFw = box.rotationUp = 0.0f;
	box.rotationR = 0.0;
	boxes.push_back(box);
	
	box.position = math::vec3(-1.0f, 8.0, 4.0f);
	box.offset = math::vec3(-1.0f, -1.0f, 0.0f);
	box.size = math::vec3(2.0, 2.0, -8.0f);
	box.textureOffset = math::vec2(9.0f, 18.0f);
	box.rotationFw = box.rotationUp = 0.0f;
	box.rotationR = 1.13f;
	boxes.push_back(box);
}

void ModelWolf::updateTextures(const ModelTextures &textures)
{
	textureOffset = textures.getTextureTile("wolf").position;
}

}
}