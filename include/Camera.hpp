#ifndef CAMERA_H
#define CAMERA_H

#include "Math.hpp"
#include <ostream>

class Camera
{
public:
    Camera();
    math::quat orientation() const;
    math::mat4 getMatrix() const;
    math::mat4 getVP() const;
    void rotate(float x, float y);
    void move(float f, float r, float u);
    math::vec3 transform(float f, float r, float u) const;
    math::vec3 untransform(const math::vec3 &v) const;

    float fov, aspectRatio, fovMultiplier;
    float zNear, zFar;
    math::vec3 position;
    math::vec3 fw, r;
    float yaw, pitch;
    float sens;
};

std::ostream &operator << (std::ostream &stream, const Camera &camera);

#endif // CAMERA_H
