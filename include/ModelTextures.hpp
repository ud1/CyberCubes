#ifndef MODEL_TEXTURES_HPP
#define MODEL_TEXTURES_HPP

#include <vector>
#include <string>
#include <GL/glew.h>
#include <map>

#include "Atlas.hpp"
#include "Math.hpp"

namespace cyberCubes
{
namespace model
{


class ModelTextures
{
public:
	ModelTextures(const ModelTextures &o) = delete;
	
	explicit ModelTextures(const math::ivec2 &size);
	atlas::Tile addTexture(const std::string &fileName, const std::string &textureName);
	atlas::Tile getTextureTile(const std::string &textureName) const;
	void upload();
	GLuint getTexture() const
	{
		return texture;
	}
	
	math::vec2 size() const
	{
		math::ivec2 sz = atlas.getSize();
		return math::vec2(sz.x, sz.y);
	}
	
	~ModelTextures();
	
private:
	atlas::Atlas atlas;
	std::vector<unsigned char> pixels;
	GLuint texture = 0;
	std::map<std::string, atlas::Tile> textures;
};

}
}

#endif
