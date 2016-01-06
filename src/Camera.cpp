#include "Camera.hpp"
#include <glm/ext.hpp>
#include <iostream>

Camera::Camera()
{
	position = math::vec3(0.0f, 0.0f, 0.0f);
	fw = math::vec3(0.0f, 1.0f, 0.0f);
	r = math::vec3(1.0f, 0.0f, 0.0f);
	yaw = pitch = 0.0f;
	pitch = 0.0f;
	fov = 80.0f / 180.0 * M_PI;
	fovMultiplier = 1.0;
	aspectRatio = 1.0f;
	zNear = 0.02f;
	zFar = 1000.0f;
	sens = 0.001f;
}

math::quat Camera::orientation() const
{
	math::vec3 up = math::cross(r, fw);
	math::quat orientation = math::angleAxis(0.0f, up);

	math::quat turn;

	turn = math::angleAxis(pitch, r);
	orientation = math::cross(turn, orientation);

	turn = math::angleAxis(-yaw, up);
	orientation = math::cross(turn, orientation);

	return orientation;
}

math::mat4 Camera::getMatrix() const
{
	math::quat orientation = this->orientation();
	math::vec3 center = orientation * fw + position;
	math::vec3 up = orientation * math::cross(r, fw);
	return math::lookAt(position, center, up);
}

math::mat4 Camera::getVP() const
{
	math::mat4 proj = math::perspective(fov * fovMultiplier, aspectRatio, zNear, zFar);
	return proj * getMatrix();
}

void Camera::rotate(float x, float y)
{
	yaw += x * sens;
	pitch -= y * sens;

	const float maxPitch = 0.499f * 3.1415926535f;

	if (pitch > maxPitch)
		pitch = maxPitch;

	if (pitch < -maxPitch)
		pitch = -maxPitch;
}

void Camera::move(float f, float r, float u)
{
	position += transform(f, r, u);
}

math::vec3 Camera::transform(float f, float r, float u) const
{
	math::quat orientation = this->orientation();

	math::vec3 vf, vu, vr;
	vf = orientation * this->fw;
	vr = orientation * this->r;
	vu = math::cross(vr, vf);

	math::vec3 res = math::vec3(0.0f, 0.0f, 0.0f);
	res += vf * f;
	res += vr * r;
	res += vu * u;

	return res;
}

math::vec3 Camera::untransform(const math::vec3 &v) const
{
	math::quat orientation = math::conjugate(this->orientation());
	math::vec3 d = orientation * v;
	math::vec3 res = math::vec3(math::dot(d, this->fw), math::dot(d, this->r), math::dot(d, math::cross(this->r, this->fw)));

	return res;
}

std::ostream &operator << (std::ostream &stream, const Camera &camera)
{
	stream << "Camera[";
	math::mat4 vp = camera.getVP();

	for (int i = 0; i < 16; ++i)
	{
		stream << (&vp[0][0])[i];

		if (i % 4 == 0)
			stream << ",";

		if (i < 15)
			stream << " ";
	}

	return stream;
}
