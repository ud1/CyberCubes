#ifndef MATH_H_INCLUDED
#define MATH_H_INCLUDED

#define GLM_FORCE_RADIANS 1
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

enum Dir
{
    XN, XP, YN, YP, ZN, ZP
};

namespace math
{
    using namespace glm;
}

#endif // MATH_H_INCLUDED
