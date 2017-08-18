#include "geometry.h"
#include "ray.h"
#include "common.h"

namespace geo {
	bool Rectangle::get_ray_intersection(const Ray &ray, double &t) {
		return getRayRectangleIntersection(ray, normal, up, position, width, height, t);
	}
}