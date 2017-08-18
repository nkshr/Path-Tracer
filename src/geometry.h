#pragma once
#include "ray.h"

namespace geo {
	struct Geometry {
	public:
		Vec position;

		virtual bool get_ray_intersection(const Ray &ray, double &t) = 0;
	};

	struct Rectangle : public Geometry {
	public:
		Vec normal;
		Vec up;

		double width;
		double height;

		virtual bool get_ray_intersection(const Ray &ray, double &t);
	};
};
