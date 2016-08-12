#ifndef MODELS_HPP
#define MODELS_HPP

#include "ModelRenderer.hpp"
#include "types.hpp"

namespace cyberCubes
{
namespace model
{

class ModelWolf : public Model
{
public:
	ModelWolf();
	virtual void updateTextures(const ModelTextures &textures) override;
};

class ModelChest : public Model
{
public:
	ModelChest();
	void setLidAngle(float angle);
	virtual void updateTextures(const ModelTextures &textures) override;
};

std::unique_ptr<Model> createModel(CubeType c);

}
}

#endif
