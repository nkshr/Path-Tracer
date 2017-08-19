#include "geometry.h"
#include "ray.h"
#include "common.h"

namespace geo {
	bool Rectangle::intersect(const Ray &ray, double &t) const {
		return calcRayRectangleIntersection(ray, position, normal, up, width, height, t);
	}
}