#ifndef BLOCK_RENDERER_H
#define BLOCK_RENDERER_H

#include <GL/glew.h>

#include "Block.hpp"
#include "Math.hpp"
#include "Shader.hpp"

namespace cyberCubes {
namespace render {
	
class BlockRenderer
{
public:
	BlockRenderer();
	void renderInventoryBlock(Block *block, const math::mat4 &mvp);
	
private:
	Shader simpleBlockShader;
	GLuint simpleBlockVao = 0;
	GLuint simpleBlockBufferObject = 0;
	
};

}}

#endif
