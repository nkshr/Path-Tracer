#include "geometry.h"
#include "ray.h"
#include "common.h"

namespace geo {
	Rectangle::Rectangle(Vec p, Vec n, Vec u, double w, double h) : 
		m_p(p), m_n(n), m_u(u), m_w(w), m_h(h){
		
	}

	bool Rectangle::get_ray_intersection(const Ray &ray, double &t) {
		return getRayRectangleIntersection(ray, m_n, m_u, m_p, m_w, m_h, t);
	}
}