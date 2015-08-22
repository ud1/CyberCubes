#ifndef BLOCK_H
#define BLOCK_H
#include <string>

// tolua_export
// tolua_begin
struct Block
{
	Block();

	int id;
	std::string name;
	std::string mainTexture;
	bool opaque;
	int lightOpacity;
	int lightValueSun, lightValueR, lightValueG, lightValueB;
	float hardness;
	
	int textureId;

	void registerBlock();
	static Block *get(int id);
	static const Block *get(const std::string &name);
};
// tolua_end

#endif // BLOCK_H
