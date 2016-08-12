#ifndef BLOCK_RENDERER_H
#define BLOCK_RENDERER_H

#include <GL/glew.h>
#include <unordered_map>

#include "ModelRenderer.hpp"
#include "Block.hpp"
#include "Math.hpp"
#include "Shader.hpp"

namespace cyberCubes {
namespace render {
	
class BlockRenderer
{
public:
	BlockRenderer(model::ModelRenderer &modelRenderer);
	void renderInventoryBlock(const Block *block, const math::mat4 &mvp);
	
private:
	Shader simpleBlockShader;
	GLuint simpleBlockVao = 0;
	GLuint simpleBlockBufferObject = 0;
	
	model::ModelRenderer &modelRenderer;
	std::unordered_map<int, cyberCubes::model::ModelRendererBuffer> modelBuffers;
};

}}

#endif
