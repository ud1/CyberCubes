#include "Block.hpp"
#include <iostream>
#include "Chunk.hpp"
#include "Math.hpp"

typedef std::unordered_map<int, Block> BlocksById;
typedef std::unordered_map<std::string, const Block *> BlocksByName;

static BlocksById blocksById;
static BlocksByName blocksByName;


Block::Block()
{
	opaque = true;
	lightOpacity = 0;
	lightValueSun = lightValueR = lightValueG = lightValueB = 0;
	hardness = 1.0;
}

void Block::registerBlock()
{
	lightValueSun = clamp(lightValueSun, 0, MAX_LIGHT);
	lightValueR   = clamp(lightValueR,   0, MAX_LIGHT);
	lightValueG   = clamp(lightValueG,   0, MAX_LIGHT);
	lightValueB   = clamp(lightValueB,   0, MAX_LIGHT);

	
	blocksById[id] = *this;
	blocksByName[name] = &blocksById[id];
	std::cout << "Register block, id = " << id << ", name = " << name << std::endl;
}

std::string Block::getTexture(int side)
{
	switch ((Dir) side)
	{
		case Dir::XN:
			if (!textureXN.empty())
				return textureXN;
			break;
		case Dir::XP:
			if (!textureXP.empty())
				return textureXP;
			break;
		case Dir::YN:
			if (!textureYN.empty())
				return textureYN;
			break;
		case Dir::YP:
			if (!textureYP.empty())
				return textureYP;
			break;
		case Dir::ZN:
			if (!textureZN.empty())
				return textureZN;
			break;
		case Dir::ZP:
			if (!textureZP.empty())
				return textureZP;
			break;
	}
	
	return mainTexture;
}

Block *Block::get(int id)
{
	BlocksById::iterator it = blocksById.find(id);
	if (it == blocksById.end())
		return nullptr;

	return &it->second;
}

const Block *Block::get(const std::string &name)
{
	BlocksByName::const_iterator it = blocksByName.find(name);
	if (it == blocksByName.end())
		return nullptr;

	return it->second;
}

const std::unordered_map<int, Block> &Block::getBlockIdMap()
{
	return blocksById;
}
