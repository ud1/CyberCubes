#include "ModelRenderer.hpp"

#include <iostream>

void checkGLError(const char *str);

namespace cyberCubes
{
namespace model
{
	
namespace {
	
void renderQuad(std::vector<Vertex> &outputBuffer, const math::vec3 &p1, const math::vec3 &p2, const math::vec3 &p3, const math::vec3 &p4, const math::vec3 &normal)
{
	Vertex vertex;
	
	vertex.position = p1;
	vertex.normal = normal;
	outputBuffer.push_back(vertex);
	
	vertex.position = p2;
	vertex.normal = normal;
	outputBuffer.push_back(vertex);
	
	vertex.position = p3;
	vertex.normal = normal;
	outputBuffer.push_back(vertex);
	
	vertex.position = p1;
	vertex.normal = normal;
	outputBuffer.push_back(vertex);
	
	vertex.position = p3;
	vertex.normal = normal;
	outputBuffer.push_back(vertex);
	
	vertex.position = p4;
	vertex.normal = normal;
	outputBuffer.push_back(vertex);
}
	
void renderBox(std::vector<Vertex> &outputBuffer, const ModelBox &box, const math::mat4 &matrix)
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
	
	normal = normalMatrix * math::vec3(0.0f, 0.0f, -1.0f);
	renderQuad(outputBuffer, p1, p2, p3, p4, normal);
	
	normal = normalMatrix * math::vec3(0.0f, 0.0f, 1.0f);
	renderQuad(outputBuffer, p8, p7, p6, p5, normal);
	
	
	normal = normalMatrix * math::vec3(0.0f, -1.0f, 0.0f);
	renderQuad(outputBuffer, p5, p6, p2, p1, normal);
	
	normal = normalMatrix * math::vec3(0.0f, 1.0f, 0.0f);
	renderQuad(outputBuffer, p7, p8, p4, p3, normal);
	
	
	normal = normalMatrix * math::vec3(-1.0f, 0.0f, 0.0f);
	renderQuad(outputBuffer, p8, p5, p1, p4, normal);
	
	normal = normalMatrix * math::vec3(1.0f, 0.0f, 0.0f);
	renderQuad(outputBuffer, p6, p7, p3, p2, normal);
	
	for (const ModelBox &child : box.children)
	{
		renderBox(outputBuffer, child, boxMatrix);
	}
}

}

ModelRendererBuffer::ModelRendererBuffer()
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
	
	glBindVertexArray(0);
}

ModelRendererBuffer::~ModelRendererBuffer()
{
	glDeleteBuffers(1, &bufferObject);
	glDeleteVertexArrays(1, &vao);
}

void ModelRendererBuffer::upload(const std::vector<Vertex> &outputBuffer)
{
	glBindBuffer(GL_ARRAY_BUFFER, bufferObject);
	glBufferData(GL_ARRAY_BUFFER, outputBuffer.size() * sizeof(Vertex), outputBuffer.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	size = outputBuffer.size();
}

void Model::render(std::vector<Vertex> &outputBuffer) const
{
	math::mat4 matrix = math::mat4(1.0f);
	matrix = math::translate(matrix, position);
	
	for (const ModelBox &child : boxes)
	{
		renderBox(outputBuffer, child, matrix);
	}
}

ModelRenderer::ModelRenderer() : modelShader("model")
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
}

void ModelRenderer::render(const ModelRendererBuffer &buffer, const Camera &camera)
{
	glBindVertexArray(buffer.vao);
	math::mat4 MVP = camera.getVP();
	math::mat4 MV = camera.getMatrix();
	
	if (modelShader.uniforms.count("MVP"))
		glUniformMatrix4fv(modelShader.uniforms["MVP"], 1, GL_FALSE, &MVP[0][0]);
		
	if (modelShader.uniforms.count("MV"))
		glUniformMatrix4fv(modelShader.uniforms["MV"], 1, GL_FALSE, &MV[0][0]);
	
	math::vec3 chunkPosition = math::vec3(0.0f, 0.0f, 0.0f);
	
	if (modelShader.uniforms.count("chunkPosition"))
		glUniform3fv(modelShader.uniforms["chunkPosition"], 1, &chunkPosition[0]);
	
	glDrawArrays(GL_TRIANGLES, 0, buffer.size);
}

void ModelRenderer::finishRender()
{
	glUseProgram(0);
	glBindVertexArray(0);
}
	
	
}}
