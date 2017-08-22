#include "geometry.h"
#include "ray.h"
#include "common.h"

namespace geo {
	bool Rectangle::intersect(const Ray &ray, double &t) const {
		return calcRayRectangleIntersection(ray, position, normal, up, width, height, t);
	}

	bool Sphere::intersect(const Ray &ray, double &t) const {
		return calcRaySphereIntersection(ray, position, radius, t);
	}

	bool Tube::intersect(const Ray &ray, double &t) const {
		return calcRayTubeIntersection(ray, position, direction, radius, height, t);
	}
}