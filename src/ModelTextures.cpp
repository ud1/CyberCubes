#include "ModelTextures.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <exception>
#include <iostream>

namespace cyberCubes
{
namespace model
{
	
class SDLSurfacePtr
{
public:
	SDL_Surface *image;
	SDLSurfacePtr(SDL_Surface *image) : image(image) {}
	~SDLSurfacePtr()
	{
		SDL_FreeSurface(image);
	}
	
};
	
ModelTextures::ModelTextures(const math::ivec2 &size) :
	atlas(size)
{
	pixels.resize(size.x * size.y * 4);
}

atlas::Tile ModelTextures::addTexture(const std::string &fileName, const std::string &textureName)
{
	SDL_Surface *image = IMG_Load(fileName.c_str());
	
	if (!image)
	{
		throw std::runtime_error(std::string("Cannot load image ") + fileName);
	}
	SDLSurfacePtr imagePtr(image);
	
	atlas::Tile tile(math::ivec2(image->w, image->h));
	
	if (!atlas.insert(tile))
	{
		throw std::runtime_error(std::string("Cannot add image ") + fileName + " to the atlas, no space available");
	}
	
	const size_t row_offset = atlas.getSize().x * 4;
	auto atlasHeight = atlas.getSize().y;
	auto bpp = image->format->BytesPerPixel;
	
	for (int y = 0; y < image->h; ++y)
	{
		unsigned char *p = pixels.data() + (tile.position.y + y) * row_offset + tile.position.x * 4; // flip texture vertically
		unsigned char *im = reinterpret_cast<unsigned char *>(image->pixels) + y * image->pitch;
		
		for (int x = 0; x < image->w; ++x)
		{
			p[4*x + 0] = im[4*x + 0];
			p[4*x + 1] = im[4*x + 1];
			p[4*x + 2] = im[4*x + 2];
			
			if (bpp == 4)
				p[4*x + 3] = im[4*x + 3];
			else
				p[4*x + 3] = 255;
		}
	}
	
	textures.insert(std::make_pair(textureName, tile));
	return tile;
}

atlas::Tile ModelTextures::getTextureTile(const std::string &textureName) const
{
	auto it = textures.find(textureName);
	if (it == textures.end())
		throw std::runtime_error(std::string("Texture tile is not found: ") + textureName);
	
	return it->second;
}

void ModelTextures::upload()
{
	if (!texture)
		glGenTextures(1, &texture);
	
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, atlas.getSize().x, atlas.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		
}

ModelTextures::~ModelTextures()
{
	glDeleteTextures(1, &texture);
}
	
}
}
