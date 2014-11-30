#include "Frustum.h"

#include <iostream>

namespace math
{

vec3 BBox::getCorner(int n) const
{
    switch (n)
    {
    case 0:
        return point;
    case 1:
        return point + vec3(size.x, 0.0f, 0.0f);
    case 2:
        return point + vec3(0.0f, size.y, 0.0f);
    case 3:
        return point + vec3(size.x, size.y, 0.0f);
    case 4:
        return point + vec3(0.0f, 0.0f, size.z);
    case 5:
        return point + vec3(size.x, 0.0f, size.z);
    case 6:
        return point + vec3(0.0f, size.y, size.z);
    case 7:
        return point + vec3(size.x, size.y, size.z);
    }
    assert(false);
    return point;
}

float BBox::calculateXOffset(const BBox &b, float offset) const
{
	const vec3 maxP = point + size;
	const vec3 &minP = point;
	const vec3 bMaxP = b.point + b.size;
	const vec3 &bMinP = b.point;
	const float EPS = 1e-2;
    if (bMaxP.y > (minP.y + EPS) && bMinP.y < (maxP.y - EPS) && bMaxP.z > (minP.z + EPS) && bMinP.z < (maxP.z - EPS))
    {
		if (offset > 0.0f && bMaxP.x <= (minP.x + EPS))
		{
			double d = minP.x - bMaxP.x;
			if (d < offset)
				offset = d;
		}

		if (offset < 0.0f && (bMinP.x + EPS) >= maxP.x)
		{
			double d = maxP.x - bMinP.x;
			if (d > offset)
				offset = d;
		}
    }

	return offset;
}

float BBox::calculateYOffset(const BBox &b, float offset) const
{
	const vec3 maxP = point + size;
	const vec3 &minP = point;
	const vec3 bMaxP = b.point + b.size;
	const vec3 &bMinP = b.point;
	const float EPS = 1e-2;
    if (bMaxP.x > (minP.x + EPS) && bMinP.x < (maxP.x - EPS) && bMaxP.z > (minP.z + EPS) && bMinP.z < (maxP.z - EPS))
    {
		if (offset > 0.0f && bMaxP.y <= (minP.y + EPS))
		{
			double d = minP.y - bMaxP.y;
			if (d < offset)
				offset = d;
		}

		if (offset < 0.0f && (bMinP.y + EPS) >= maxP.y)
		{
			double d = maxP.y - bMinP.y;
			if (d > offset)
				offset = d;
		}
    }

	return offset;
}

float BBox::calculateZOffset(const BBox &b, float offset) const
{
	const vec3 maxP = point + size;
	const vec3 &minP = point;
	const vec3 bMaxP = b.point + b.size;
	const vec3 &bMinP = b.point;
	const float EPS = 1e-2;
    if (bMaxP.x > (minP.x + EPS) && bMinP.x < (maxP.x - EPS) && bMaxP.y > (minP.y + EPS) && bMinP.y < (maxP.y - EPS))
    {
		if (offset > 0.0f && bMaxP.z <= (minP.z + EPS))
		{
			double d = minP.z - bMaxP.z;
			if (d < offset)
				offset = d;
		}

		if (offset < 0.0f && (bMinP.z + EPS) >= maxP.z)
		{
			double d = maxP.z - bMinP.z;
			if (d > offset)
				offset = d;
		}
    }

	return offset;
}

IntersectionType Plane::checkBBox(const BBox &box) const
{
    bool isInside = false;
    bool isOutside = false;
    for (int i = 0; i < 8; ++i)
    {
        float r = dot(normal, box.getCorner(i)) + dist;
        if (r <= 0)
        {
            isOutside = true;
        }
        else
        {
            isInside = true;
        }
    }
    if (!isInside)
        return OUTSIDE;
    if (!isOutside)
        return INSIDE;
    return INTERSECTING;
}


void Frustum::update(const mat4 &MVP)
{
    const float *mat = &MVP[0][0];
    float t;
    // Right plane
    planes[0].normal[0] = mat[ 3] - mat[ 0];
    planes[0].normal[1] = mat[ 7] - mat[ 4];
    planes[0].normal[2] = mat[11] - mat[ 8];
    planes[0].dist = mat[15] - mat[12];

    t = 1.0f / length(planes[0].normal);
    planes[0].normal *= t;
    planes[0].dist *= t;

    // Left plane
    planes[1].normal[0] = mat[ 3] + mat[ 0];
    planes[1].normal[1] = mat[ 7] + mat[ 4];
    planes[1].normal[2] = mat[11] + mat[ 8];
    planes[1].dist = mat[15] + mat[12];

    t = 1.0f / length(planes[1].normal);
    planes[1].normal *= t;
    planes[1].dist *= t;

    // Lower plane
    planes[2].normal[0] = mat[ 3] + mat[ 1];
    planes[2].normal[1] = mat[ 7] + mat[ 5];
    planes[2].normal[2] = mat[11] + mat[ 9];
    planes[2].dist = mat[15] + mat[13];

    t = 1.0f / length(planes[2].normal);
    planes[2].normal *= t;
    planes[2].dist *= t;

    // Upper plane
    planes[3].normal[0] = mat[ 3] - mat[ 1];
    planes[3].normal[1] = mat[ 7] - mat[ 5];
    planes[3].normal[2] = mat[11] - mat[ 9];
    planes[3].dist = mat[15] - mat[13];

    t = 1.0f / length(planes[3].normal);
    planes[3].normal *= t;
    planes[3].dist *= t;

    // Far plane
    planes[4].normal[0] = mat[ 3] - mat[ 2];
    planes[4].normal[1] = mat[ 7] - mat[ 6];
    planes[4].normal[2] = mat[11] - mat[10];
    planes[4].dist = mat[15] - mat[14];

    t = 1.0f / length(planes[4].normal);
    planes[4].normal *= t;
    planes[4].dist *= t;

    // Near  plane
    planes[5].normal[0] = mat[ 3] + mat[ 2];
    planes[5].normal[1] = mat[ 7] + mat[ 6];
    planes[5].normal[2] = mat[11] + mat[10];
    planes[5].dist = mat[15] + mat[14];

    t = 1.0f / length(planes[5].normal);
    planes[5].normal *= t;
    planes[5].dist *= t;
}

bool Frustum::isPointInFrustum(const vec3 &point) const
{
    for (int i = 0; i < 6; ++i)
    {
        if (dot(planes[i].normal, point) + planes[i].dist <= 0)
        {
            return false;
        }
    }
    return true;
}

IntersectionType Frustum::checkBBox(const BBox &box) const
{
    bool inside = true;
    for (int i = 0; i < 6; ++i)
    {
        IntersectionType r = planes[i].checkBBox(box);
        if (r == OUTSIDE)
            return OUTSIDE;
        if (r == INTERSECTING)
            inside = false;
    }
    if (inside)
        return INSIDE;
    return INTERSECTING;
}
}
