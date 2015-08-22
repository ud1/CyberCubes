#include "Block.hpp"
#include <unordered_map>
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
