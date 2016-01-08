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

inline math::vec3 dirToVec(Dir dir)
{
	switch (dir)
	{
		case Dir::XN:
			return math::vec3(-1.0f, 0.0f, 0.0f);
		case Dir::XP:
			return math::vec3(1.0f, 0.0f, 0.0f);
		case Dir::YN:
			return math::vec3(0.0f, -1.0f, 0.0f);
		case Dir::YP:
			return math::vec3(0.0f, 1.0f, 0.0f);
		case Dir::ZN:
			return math::vec3(0.0f, 0.0f, -1.0f);
		case Dir::ZP:
			return math::vec3(0.0f, 0.0f, 1.0f);
	}
	
	assert(false && "Invalid dir");
}

inline Dir vecToDir(const math::vec3 &v)
{
	float threshold = 0.9f;
	
	if (v.x > threshold)
		return Dir::XP;
	
	if (v.y > threshold)
		return Dir::YP;
	
	if (v.z > threshold)
		return Dir::ZP;
	
	if (v.x < -threshold)
		return Dir::XN;
	
	if (v.y < -threshold)
		return Dir::YN;
	
	if (v.z < -threshold)
		return Dir::ZN;
	
	assert(false && "Invalid vector");
}

inline math::mat3 textureMatrixToMat3(const math::mat2 &texMat, Dir dir)
{
	if (dir == Dir::XN || dir == Dir::XP)
	{
		return math::mat3(0.0f, 0.0f, 0.0f,  0.0f, texMat[0][0], texMat[0][1],  0.0f, texMat[1][0], texMat[1][1]);
	}
	
	if (dir == Dir::YN || dir == Dir::YP)
	{
		return math::mat3(texMat[0][0], 0.0f, texMat[0][1],  0.0f,  0.0f, 0.0f,  texMat[1][0], 0.0f, texMat[1][1]);
	}
	
	if (dir == Dir::ZN || dir == Dir::ZP)
	{
		return math::mat3(texMat[0][0], texMat[0][1], 0.0f,  texMat[1][0], texMat[1][1], 0.0f,  0.0f,  0.0f, 0.0f);
	}
}

struct RotationMatrices
{
	math::mat3 matrices[48];
	math::mat3 invMatrices[48];
	Dir invDirs[48 * 6];
	unsigned char quadRotations[48 * 6];
	
	RotationMatrices();
	
	Dir invRotate(Dir origDir, int rot) const
	{
		return invDirs[rot * 6 + (int) origDir];
	}
	
	unsigned char getQuadRotation(Dir origDir, int rot) const
	{
		return quadRotations[rot * 6 + (int) origDir];
	}
	
	static const RotationMatrices &getInstance();
};

int getRotation(const math::vec3 &cameraFwDir, const math::vec3 &cameraUpDir, int rotationGroup);

#endif // MATH_H_INCLUDED
