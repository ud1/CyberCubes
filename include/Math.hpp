#ifndef MATH_H_INCLUDED
#define MATH_H_INCLUDED

#define GLM_FORCE_RADIANS 1
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

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

struct FloatSmoothing
{
	float value, period;

	FloatSmoothing(float value, float period) : value(value), period(period) {}

	float set(float new_val, float dt)
	{
		value += (new_val - value) * (1.0 - std::exp(-dt / period));
		return value;
	}
};

#endif // MATH_H_INCLUDED
