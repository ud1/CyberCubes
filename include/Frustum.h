#ifndef FRUSTUM_H
#define FRUSTUM_H

#include "Math.h"

namespace math
{

struct BBox
{
    // Corner and box size
    BBox(const vec3 &point, const vec3 &size) : point(point), size(size) {}
    vec3 getCorner(int n) const;
    float calculateXOffset(const BBox &b, float offset) const;
    float calculateYOffset(const BBox &b, float offset) const;
    float calculateZOffset(const BBox &b, float offset) const;

    vec3 point, size;
};

enum IntersectionType
{
    INSIDE,
    OUTSIDE,
    INTERSECTING,
};

struct Plane
{
    vec3 normal;
    float dist;

    IntersectionType checkBBox(const BBox &box) const;
};

struct Frustum
{
    Plane planes[6];

    void update(const mat4 &MVP);

    bool isPointInFrustum(const vec3 &point) const;

    IntersectionType checkBBox(const BBox &box) const;
};

}
#endif // FRUSTUM_H
