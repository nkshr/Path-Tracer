#pragma once
#include "ray.h"

namespace geo {
	struct Geometry {
	public:
		Vec position;

		virtual bool intersect(const Ray &ray, double &t) const = 0;
	};

	struct Rectangle : public Geometry {
	public:
		Vec normal;
		Vec up;

		double width;
		double height;

		virtual bool intersect(const Ray &ray, double &t) const;
	};

	struct Sphere : public Geometry {
	public:
		double radius;

		virtual bool intersect(const Ray &ray, double &t) const;
	};
};
