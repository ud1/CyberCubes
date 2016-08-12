#include "ModelRenderer.hpp"

#include <iostream>

void checkGLError(const char *str);

namespace cyberCubes
{
namespace model
{
	
namespace {
	
void renderQuad(std::vector<Vertex> &outputBuffer, const math::vec3 &p1, const math::vec3 &p2, const math::vec3 &p3, const math::vec3 &p4, const math::vec3 &normal,
	const math::vec2 &textureOffset, const math::vec2 &quadSize, const math::vec2 &invTextureSize, const unsigned char *colors)
{
	Vertex vertex;
	vertex.colors[0] = colors[0];
	vertex.colors[1] = colors[1];
	vertex.colors[2] = colors[2];
	vertex.colors[3] = colors[3];
	
	vertex.position = p1;
	vertex.normal = normal;
	vertex.textureCoords = textureOffset;
	vertex.textureCoords *= invTextureSize;
	outputBuffer.push_back(vertex);
	
	vertex.position = p2;
	vertex.normal = normal;
	vertex.textureCoords = textureOffset + math::vec2(quadSize.x, 0.0f);
	vertex.textureCoords *= invTextureSize;
	outputBuffer.push_back(vertex);
	
	vertex.position = p3;
	vertex.normal = normal;
	vertex.textureCoords = textureOffset + quadSize;
	vertex.textureCoords *= invTextureSize;
	outputBuffer.push_back(vertex);
	
	vertex.position = p1;
	vertex.normal = normal;
	vertex.textureCoords = textureOffset;
	vertex.textureCoords *= invTextureSize;
	outputBuffer.push_back(vertex);
	
	vertex.position = p3;
	vertex.normal = normal;
	vertex.textureCoords = textureOffset + quadSize;
	vertex.textureCoords *= invTextureSize;
	outputBuffer.push_back(vertex);
	
	vertex.position = p4;
	vertex.normal = normal;
	vertex.textureCoords = textureOffset + math::vec2(0.0f, quadSize.y);
	vertex.textureCoords *= invTextureSize;
	outputBuffer.push_back(vertex);
}
	
void renderBox(std::vector<Vertex> &outputBuffer, const ModelBox &box, const math::mat4 &matrix, const math::vec2 &modelTextureOffset, const math::vec2 &invTextureSize, const unsigned char *colors)
{
	math::mat4 boxMatrix;
	
	boxMatrix = math::translate(matrix, box.position);
	boxMatrix = math::rotate(boxMatrix, box.rotationUp, glm::vec3(0.0f, 0.0f, 1.0f));
	boxMatrix = math::rotate(boxMatrix, box.rotationFw, glm::vec3(0.0f, 1.0f, 0.0f));
	boxMatrix = math::rotate(boxMatrix, box.rotationR, glm::vec3(1.0f, 0.0f, 0.0f));
	boxMatrix = math::translate(boxMatrix, box.offset);
	
	
	math::vec3 p1 = math::vec3(boxMatrix * math::vec4(0.0f,       0.0f,       0.0f,       1.0f));
	math::vec3 p2 = math::vec3(boxMatrix * math::vec4(box.size.x, 0.0f,       0.0f,       1.0f));
	math::vec3 p3 = math::vec3(boxMatrix * math::vec4(box.size.x, box.size.y, 0.0f,       1.0f));
	math::vec3 p4 = math::vec3(boxMatrix * math::vec4(0.0f,       box.size.y, 0.0f,       1.0f));
	
	math::vec3 p5 = math::vec3(boxMatrix * math::vec4(0.0f,       0.0f,       box.size.z, 1.0f));
	math::vec3 p6 = math::vec3(boxMatrix * math::vec4(box.size.x, 0.0f,       box.size.z, 1.0f));
	math::vec3 p7 = math::vec3(boxMatrix * math::vec4(box.size.x, box.size.y, box.size.z, 1.0f));
	math::vec3 p8 = math::vec3(boxMatrix * math::vec4(0.0f,       box.size.y, box.size.z, 1.0f));
	
	math::vec3 normal;
	
	math::mat3 normalMatrix = math::mat3(boxMatrix);
	
	math::vec3 sz = math::abs(box.size);
	
	normal = math::normalize(normalMatrix * math::vec3(0.0f, 0.0f, -1.0f));
	renderQuad(outputBuffer, p1, p2, p3, p4, normal, modelTextureOffset + box.textureOffset + math::vec2(sz.y + sz.x, sz.y), math::vec2(sz.x, -sz.y), invTextureSize, colors);
	
	normal = math::normalize(normalMatrix * math::vec3(0.0f, 0.0f, 1.0f));
	renderQuad(outputBuffer, p8, p7, p6, p5, normal, modelTextureOffset + box.textureOffset + math::vec2(sz.y, 0.0f), math::vec2(sz.x, sz.y), invTextureSize, colors);
	
	
	normal = math::normalize(normalMatrix * math::vec3(0.0f, -1.0f, 0.0f));
	renderQuad(outputBuffer, p5, p6, p2, p1, normal, modelTextureOffset + box.textureOffset + math::vec2(sz.y, sz.y), math::vec2(sz.x, sz.z), invTextureSize, colors);
	
	normal = math::normalize(normalMatrix * math::vec3(0.0f, 1.0f, 0.0f));
	renderQuad(outputBuffer, p7, p8, p4, p3, normal, modelTextureOffset + box.textureOffset + math::vec2(2.0f*sz.y + sz.x, sz.y), math::vec2(sz.x, sz.z), invTextureSize, colors);
	
	
	normal = math::normalize(normalMatrix * math::vec3(-1.0f, 0.0f, 0.0f));
	renderQuad(outputBuffer, p8, p5, p1, p4, normal, modelTextureOffset + box.textureOffset + math::vec2(0.0f, sz.y), math::vec2(sz.y, sz.z), invTextureSize, colors);
	
	normal = math::normalize(normalMatrix * math::vec3(1.0f, 0.0f, 0.0f));
	renderQuad(outputBuffer, p6, p7, p3, p2, normal, modelTextureOffset + box.textureOffset + math::vec2(sz.y + sz.x, sz.y), math::vec2(sz.y, sz.z), invTextureSize, colors);
	
	for (const ModelBox &child : box.children)
	{
		renderBox(outputBuffer, child, boxMatrix, modelTextureOffset, invTextureSize, colors);
	}
}

}

ModelRendererBuffer::ModelRendererBuffer()
{
}

ModelRendererBuffer::~ModelRendererBuffer()
{
	if (vao)
	{
		glDeleteBuffers(1, &bufferObject);
		glDeleteVertexArrays(1, &vao);
	}
}

void ModelRendererBuffer::upload(const std::vector<Vertex> &outputBuffer)
{
	if (!vao)
	{
		glGenBuffers(1, &bufferObject);
	
		glGenVertexArrays(1, &vao);
		
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, bufferObject);
		
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), &((Vertex *)nullptr)->position);
		
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), &((Vertex *)nullptr)->normal);
		
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), &((Vertex *)nullptr)->textureCoords);
		
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), &((Vertex *)nullptr)->colors[0]);
		
		glBindVertexArray(0);
	}
	
	glBindBuffer(GL_ARRAY_BUFFER, bufferObject);
	glBufferData(GL_ARRAY_BUFFER, outputBuffer.size() * sizeof(Vertex), outputBuffer.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	size = outputBuffer.size();
}

Model::Model() : colors{255, 255, 255, 255}
{
}

void Model::render(std::vector<Vertex> &outputBuffer, const math::vec2 &invTextureSize) const
{
	math::mat4 matrix = math::mat4(1.0f);
	matrix = math::translate(matrix, position);
	matrix = math::scale(matrix, math::vec3(1.0f, 1.0f, 1.0f)/16.0f);
	
	const math::mat3 &rot = RotationMatrices::getInstance().matrices[rotation];
	const math::mat4 rot4 = math::mat4(rot);
	matrix = matrix*rot4;
	
	for (const ModelBox &child : boxes)
	{
		renderBox(outputBuffer, child, matrix, textureOffset, invTextureSize, colors);
	}
}

ModelRenderer::ModelRenderer(const ModelTextures &modelTextures) : modelShader("model"), modelTextures(modelTextures)
{
	modelShader.buildShaderProgram("modelvs.glsl", "modelfs.glsl");
}

ModelRenderer::~ModelRenderer()
{
	
}

void ModelRenderer::startRender()
{
	glUseProgram(modelShader.program);
	
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	
	if (modelShader.uniforms.count("modelTextures"))
	{
		glUniform1i(modelShader.uniforms["modelTextures"], 0);
		glActiveTexture(GL_TEXTURE0 + 0);
		glBindTexture(GL_TEXTURE_2D, modelTextures.getTexture());
	}
}

void ModelRenderer::render(const ModelRendererBuffer &buffer, const Camera &camera)
{
	math::mat4 MVP = camera.getVP();
	render(buffer, MVP);
}

void ModelRenderer::render(const ModelRendererBuffer &buffer, const math::mat4 &MVP)
{
	glBindVertexArray(buffer.vao);
	
	if (modelShader.uniforms.count("MVP"))
		glUniformMatrix4fv(modelShader.uniforms["MVP"], 1, GL_FALSE, &MVP[0][0]);
	
	glDrawArrays(GL_TRIANGLES, 0, buffer.size);
}

void ModelRenderer::finishRender()
{
	glUseProgram(0);
	glBindVertexArray(0);
}
	
	
}}
