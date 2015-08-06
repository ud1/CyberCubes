#ifndef MATH_H_INCLUDED
#define MATH_H_INCLUDED

#define GLM_FORCE_RADIANS 1
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

enum class Dir
{
    XN, XP, YN, YP, ZN, ZP
};

namespace math
{
    using namespace glm;
}

template<typename T>
T clamp(T val, T minV, T maxV)
{
	if (val < minV)
		val = minV;
	if (val > maxV)
		val = maxV;
	return val;
}

#endif // MATH_H_INCLUDED
