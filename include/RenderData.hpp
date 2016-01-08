#ifndef RENDERDATA_H
#define RENDERDATA_H

#include <GL/glew.h>
#include <vector>
#include <map>
#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_comparison.hpp>
#include <stdint.h>
#include "Math.hpp"
#include "Chunk.hpp"

struct Vertex
{
	signed char x, y, z;
	unsigned char normalIndex;
	unsigned char colorH, colorS;
	unsigned char rotation;
	int16_t textureId;
	unsigned char l1, l2, l3, l4;
	unsigned char sl1, sl2, sl3, sl4;
};

struct RenderData
{
	~RenderData();
	
	std::vector<Vertex> opaqueVertices[6];
	std::vector<int> opaqueLayerIndices[6];
	std::vector<uint16_t> nonOpaqueIndices[8];
	
	std::vector<Vertex> nonOpaqueVertices;
	int trisRendered;
	
	GLuint opaqueVao = 0;
	GLuint nonOpaqueVao = 0;
	GLuint triangleBufferObject = 0;
	GLuint indexBufferObject = 0;

	size_t addFace(const math::ivec3 &pos, int textureId, Dir dir, const Chunk &chunk, unsigned char quadRotation, bool opaque);
	void uploadData();
	size_t getVideoMemUse() const;
	void render(GLint clipDirLocation, const math::vec3 &eye, Tick tick, bool opaque);
	void addVertexData(const Chunk &chunk);

	template<LightType lt>
	LightValF<lt> lightAt(const Chunk &chunk, const math::ivec3 &pos, const math::ivec3 &from);
	
	Tick getLastRenderTick() const
	{
		return _renderTick;
	}
	
private:
	Tick _renderTick = 0;
};

#endif // RENDERDATA_H
