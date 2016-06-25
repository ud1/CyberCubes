#ifndef MODELRENDERER_H
#define MODELRENDERER_H

#include "Math.hpp"
#include "Shader.hpp"
#include "Camera.hpp"
#include <vector>

#include <GL/glew.h>

namespace cyberCubes
{
namespace model
{

struct Vertex
{
	math::vec3 position, normal;
	math::vec2 textureCoords;
};

struct ModelBox
{
	math::vec2 textureOffset;
	math::vec3 offset, position, size;
	float rotationUp, rotationFw, rotationR;
	
	std::vector<ModelBox> children;
};

struct Model
{
	math::vec2 textureOffset, textureSize;
	math::vec3 position;
	std::vector<ModelBox> boxes;
	
	void render(std::vector<Vertex> &outputBuffer) const;
};

struct ModelRendererBuffer
{
	ModelRendererBuffer();
	~ModelRendererBuffer();
	
	void upload(const std::vector<Vertex> &outputBuffer);
	
	GLuint bufferObject;
	GLuint vao;
	size_t size;
};

class ModelRenderer
{
public:
	ModelRenderer();
	~ModelRenderer();
	
	void startRender();
	void render(const ModelRendererBuffer &buffer, const Camera &camera);
	void finishRender();
	
private:
	Shader modelShader;
};

}}

#endif
