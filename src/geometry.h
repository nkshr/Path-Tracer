#pragma once
#include "ray.h"

namespace geo {
	class Geometry {
		virtual bool get_ray_intersection(const Ray &ray, double &t) = 0;
	};

	class Rectangle : public Geometry {
	private:
		Vec m_p;
		Vec m_n;
		Vec m_u;
		Vec m_t;

		Vec m_x_dir;
		Vec m_y_dir;

		double m_w;
		double m_h;

	public:
		Rectangle(Vec p, Vec n, Vec up, double w, double h);
		virtual bool get_ray_intersection(const Ray &ray, double &t);
	};
};
