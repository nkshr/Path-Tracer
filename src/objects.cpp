#include <math.h>
#include <iostream>
#include <stdio.h>

#include "vector.h"
#include "ray.h"
#include "material.h"
#include "objects.h"
//#include "../lib/fastbvh/BVH.h"


ObjectIntersection::ObjectIntersection(bool hit_, double u_, Vec n_, Material m_)
{
	hit=hit_, u=u_, n=n_, m=m_;
}


Sphere::Sphere( Vec p_, double r_, Material m_ ) {
	m_p=p_, m_r=r_, m_m=m_;
}

double Sphere::get_radius() { return m_r; }
Material Sphere::get_material() { return m_m; }

// Check if ray intersects with sphere. Returns ObjectIntersection data structure
ObjectIntersection Sphere::get_intersection(const Ray &ray) {
	// Solve t^2*d.d + 2*t*(o-p).d + (o-p).(o-p)-R^2 = 0
	bool hit = false;
	double distance = 0;
	Vec n = Vec();

	Vec op = m_p-ray.origin;
	double t, eps=1e-4, b=op.dot(ray.direction), det=b*b-op.dot(op)+m_r*m_r;
	if (det<0) return ObjectIntersection(hit, distance, n, m_m); 
	else det=sqrt(det);
	distance = (t=b-det)>eps ? t : ((t=b+det)>eps ? t : 0);
	if (distance != 0) hit = true, 
		n = ((ray.origin + ray.direction * distance) - m_p).norm();

	return ObjectIntersection(hit, distance, n, m_m);
}

Vec Object::get_position() {
	return m_p;
}

Material Object::get_material() {
	return m_m;
}