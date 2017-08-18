#include "geometry.h"
#include "ray.h"
#include "common.h"

namespace geo {
	bool Rectangle::intersect(const Ray &ray, double &t) const {
		return calcRayRectangleIntersection(ray, normal, up, position, width, height, t);
	}
}