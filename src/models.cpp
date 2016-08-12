#include "models.hpp"
#include "blockType.hpp"

namespace cyberCubes
{
namespace model
{
	
std::unique_ptr<Model> createModel(CubeType c)
{
	switch(c)
	{
	case BLOCK_CHEST:
		return std::unique_ptr<Model>(new ModelChest);
	}
	
	return std::unique_ptr<Model>();
}

}
}