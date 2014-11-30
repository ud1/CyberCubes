#include "WorldProvider.h"

#include <fstream>
#include <sstream>
#include <string>

std::string getFileName(const math::ivec3 &pos)
{
	std::ostringstream oss;
	oss << "world/" << (pos.x > 0 ? "p" : "n") << pos.x << (pos.y > 0 ? "p" : "n") << pos.y << (pos.z > 0 ? "p" : "n") << pos.z << ".wdat";
	return oss.str();
}

bool WorldProvider::save(const Chunk &chunk, const math::ivec3 &pos, bool rewrite)
{
	if (chunk.isDummy)
		return false;

	std::string fileName = getFileName(pos);

	if (!rewrite)
	{
		std::ifstream file(getFileName(pos), std::ios::binary);
		if (file)
			return false;
	}

	std::ofstream file(getFileName(pos), std::ios::binary);

	file.write((char *) chunk.cubes, sizeof(chunk.cubes));
	file.write((char *) chunk.light, sizeof(chunk.light));

	return true;
}

bool WorldProvider::load(Chunk &chunk, const math::ivec3 &pos)
{
	std::ifstream file(getFileName(pos), std::ios::binary);
	if (!file)
		return false;

	file.read((char *) chunk.cubes, sizeof(chunk.cubes));
	file.read((char *) chunk.light, sizeof(chunk.light));

	return true;
}
