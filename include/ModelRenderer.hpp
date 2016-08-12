#ifndef MODELRENDERER_H
#define MODELRENDERER_H

#include "Math.hpp"
#include "Shader.hpp"
#include "Camera.hpp"
#include "ModelTextures.hpp"
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
	unsigned char colors[4]; // sun/r/g/b
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
	Model();
	virtual ~Model() {}
	
	math::vec2 textureOffset;
	math::vec3 position;
	std::vector<ModelBox> boxes;
	int rotation = 0;
	unsigned char colors[4]; // sun/r/g/b
	
	virtual void updateTextures(const ModelTextures &textures) = 0;
	void render(std::vector<Vertex> &outputBuffer, const math::vec2 &invTextureSize) const;
};

struct ModelRendererBuffer
{
	ModelRendererBuffer();
	~ModelRendererBuffer();
	
	void upload(const std::vector<Vertex> &outputBuffer);
	
	GLuint bufferObject;
	GLuint vao = 0;
	size_t size;
};

class ModelRenderer
{
public:
	ModelRenderer(const ModelTextures &modelTextures);
	~ModelRenderer();
	
	void startRender();
	void render(const ModelRendererBuffer &buffer, const Camera &camera);
	void render(const ModelRendererBuffer &buffer, const math::mat4 &MVP);
	void finishRender();
	math::vec2 getInvTextureSize() const
	{
		return 1.0f / modelTextures.size();
	}
	
	const ModelTextures &getModelTextures() const
	{
		return modelTextures;
	}
private:
	Shader modelShader;
	const ModelTextures &modelTextures;
};

}}

#endif
