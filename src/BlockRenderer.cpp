#include "BlockRenderer.hpp"
#include "Math.hpp"
#include "Chunk.hpp"
#include <cstdint>

extern GLuint blockTexture;

namespace cyberCubes {
namespace render {

struct SimpleBlockVertex
{
	unsigned char normalIndex;
	int16_t textureId;
	unsigned char light[4];
};
	
BlockRenderer::BlockRenderer() : simpleBlockShader("simpleBlockShader")
{
	simpleBlockShader.buildShaderProgram("simpleBlockvs.glsl", "simpleBlockgs.glsl", "simpleBlockfs.glsl");
	
	glGenVertexArrays(1, &simpleBlockVao);
	glGenBuffers(1, &simpleBlockBufferObject);

	glBindVertexArray(simpleBlockVao);
	glBindBuffer(GL_ARRAY_BUFFER, simpleBlockBufferObject);
	
	glEnableVertexAttribArray(0);
	glVertexAttribIPointer(0, 1, GL_UNSIGNED_BYTE, sizeof(SimpleBlockVertex), &((SimpleBlockVertex *)nullptr)->normalIndex);
	
	glEnableVertexAttribArray(1);
	glVertexAttribIPointer(1, 1, GL_SHORT, sizeof(SimpleBlockVertex), &((SimpleBlockVertex *)nullptr)->textureId);
	
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(SimpleBlockVertex), &((SimpleBlockVertex *)nullptr)->light);
	
	glBindVertexArray(0);
}

void BlockRenderer::renderInventoryBlock(const Block *block, const math::mat4 &mvp)
{
	SimpleBlockVertex vertexData[6];
	
	unsigned char d = 25;
	unsigned char l1 = 255;
	unsigned char l2 = 255 - d;
	unsigned char l3 = 255 - 2*d;
	unsigned char l4 = 255 - 3*d;
	
	vertexData[0].normalIndex = (unsigned char) Dir::XN;
	vertexData[0].textureId = block->textureId[(int) Dir::XN];
	vertexData[0].light[0] = l4;
	vertexData[0].light[1] = l4;
	vertexData[0].light[2] = l4;
	vertexData[0].light[3] = l4;
	
	vertexData[1].normalIndex = (unsigned char) Dir::YP;
	vertexData[1].textureId = block->textureId[(int) Dir::YP];
	vertexData[1].light[0] = l4;
	vertexData[1].light[1] = l4;
	vertexData[1].light[2] = l4;
	vertexData[1].light[3] = l4;
	
	vertexData[2].normalIndex = (unsigned char) Dir::ZN;
	vertexData[2].textureId = block->textureId[(int) Dir::ZN];
	vertexData[2].light[0] = l4;
	vertexData[2].light[1] = l4;
	vertexData[2].light[2] = l4;
	vertexData[2].light[3] = l4;
	
	vertexData[3].normalIndex = (unsigned char) Dir::XP;
	vertexData[3].textureId = block->textureId[(int) Dir::XP];
	vertexData[3].light[0] = l3;
	vertexData[3].light[1] = l3;
	vertexData[3].light[2] = l3;
	vertexData[3].light[3] = l3;
	
	vertexData[4].normalIndex = (unsigned char) Dir::YN;
	vertexData[4].textureId = block->textureId[(int) Dir::YN];
	vertexData[4].light[0] = l2;
	vertexData[4].light[1] = l2;
	vertexData[4].light[2] = l2;
	vertexData[4].light[3] = l2;
	
	vertexData[5].normalIndex = (unsigned char) Dir::ZP;
	vertexData[5].textureId = block->textureId[(int) Dir::ZP];
	vertexData[5].light[0] = l1;
	vertexData[5].light[1] = l1;
	vertexData[5].light[2] = l1;
	vertexData[5].light[3] = l1;
	
	glBindBuffer(GL_ARRAY_BUFFER, simpleBlockBufferObject);
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(SimpleBlockVertex), vertexData, GL_STREAM_DRAW);
	
	glBindVertexArray(simpleBlockVao);
	
	glUseProgram(simpleBlockShader.program);

	if (simpleBlockShader.uniforms.count("MVP"))
	{	
		glUniformMatrix4fv(simpleBlockShader.uniforms["MVP"], 1, GL_FALSE, &mvp[0][0]);
	}
	
	if (simpleBlockShader.uniforms.count("blockSampler"))
	{
		glUniform1i(simpleBlockShader.uniforms["blockSampler"], 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D_ARRAY, blockTexture);
	}
	
	if (simpleBlockShader.uniforms.count("lightColor"))
	{	
		math::vec3 color = math::vec3(block->lightValueR, block->lightValueG, block->lightValueB) / MAX_LIGHT_F;
		if (!block->lightValueR && !block->lightValueG && !block->lightValueB)
			color = math::vec3(1.0f, 1.0f, 1.0f);
		
		glUniform3fv(simpleBlockShader.uniforms["lightColor"], 1, &color[0]);
	}
	
	
	
	glEnable(GL_DEPTH_TEST);
	glDrawArrays(GL_POINTS, 0, 6);
}


}}