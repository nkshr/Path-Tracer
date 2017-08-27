#include "geometry.h"
#include "ray.h"
#include "common.h"

namespace geo {
	bool Rectangle::intersect(const Ray &ray, Vec &n, double &t) const {
		bool calculated =  calcRayRectangleIntersection(ray, position, normal, up, width, height, t);
		n = normal;
		return calculated;
	}

	bool Sphere::intersect(const Ray &ray, Vec &n, double &t) const {
		bool calculated =  calcRaySphereIntersection(ray, position, radius, t);
		
		Vec isct = ray.origin + ray.direction * t;
		n = (isct - position).norm();
		
		return calculated;
	}

	bool Tube::intersect(const Ray &ray,  Vec &n, double &t) const {
		return calcRayTubeIntersection(ray, position, direction, radius, height, n, t);
	}

	bool Disc::intersect(const Ray &ray, Vec &n, double &t) const {
		bool calculated =  calcRayDiscIntersection(ray, position, normal, radius, t);
		n = normal;
		return calculated;
	}
}