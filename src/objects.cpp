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

Cylinder::Cylinder(Vec p_, Vec d_, double r_, double h_, Material m_) {
	m_p = p_;
	m_d = d_.norm();
	m_r = r_;
	m_h = h_;
	m_m = m_;
}

double Cylinder::get_radius() {
	return m_r;
}

double Cylinder::get_height() {
	return m_h;
}

Vec Cylinder::get_direction() {
	return m_d;
}

//http://mrl.nyu.edu/~dzorin/rend05/lecture2.pdf
ObjectIntersection Cylinder::get_intersection(const Ray&ray) {
	const double hhalf = m_h * 0.5;
	const Vec p0 = m_p - m_d * hhalf;
	const Vec p1 = m_p + m_d * hhalf;

	const Vec x0 = ray.origin - p0;
	const Vec x1 = ray.direction - m_d * ray.direction.dot(m_d);
	const Vec x2 = x0 - m_d * x0.dot(m_d);

	const double a = x1.dot(x1);
	const double b = x1.dot(x2) * 2.0;
	const double c = x2.dot(x2) - m_r * m_r;

	std::vector<std::pair<double, Vec> > tns;
	const double d = b*b - 4 * a * c;

	if (!(d < 0)) {
		const double sqrt_d = sqrt(d);
		const double a2_recip = 1.0 / (2.0 * a);
        double t = (-b + sqrt_d) * a2_recip;
		
		if (!(t < 0)) {
			Vec q = ray.origin + ray.direction * t;

			if (m_d.dot(q - p0) > 0 && m_d.dot(q - p1) < 0) {
			  Vec n = (q - m_d * q.dot(m_d)).norm();
			  tns.push_back(std::pair<double, Vec>(t, n));
			}
		}
		
		t = (-b - sqrt_d) * a2_recip;

		if (!(t < 0)) {
			Vec q = ray.origin + ray.direction * t;

			if (m_d.dot(q - p0) > 0 && m_d.dot(q - p1) < 0) {
			  Vec n = (q - m_d * q.dot(m_d)).norm();
			  tns.push_back(std::pair<double, Vec>(t, n));
			}
		}
	}

	double t  = (m_d.dot(p0 - ray.origin)) / (m_d.dot(ray.direction));
	double r2 = m_r * m_r;
	if (!(t < 0)) {
		Vec q = ray.origin + ray.direction * t;
		Vec qp = q - p0;
		if (qp.dot(qp) < r2) {
		  Vec n = m_d;
		  tns.push_back(std::pair<double, Vec>(t, n));
		}
	}

	t = (m_d.dot(p1 - ray.origin)) / (m_d.dot(ray.direction));
	if (!(t < 0)) {
		Vec q = ray.origin + ray.direction * t;
		Vec qp = q - p1;
		if (qp.dot(qp) < r2) {
		  Vec n = m_d*(-1);
		  tns.push_back(std::pair<double, Vec>(t, n));
		}
	}

	if(!tns.size()){
	  return ObjectIntersection(false, 0.0, Vec(), m_m);
	}

	if(tns.size() >= 3){
	  std::cout << "tns.size() : " << tns.size() << std::endl;
	  return -1;
	}

	std::pair<double, Vec> tn = tns[0];
	if(tns.size() == 2){
	  if(tn.first > tns[1].first){
	    tn = tns[1];
	  }
	  tn.second = tn.second * (-1.0);
	  std::cout << "tns.size() : " << tns.size() << std::endl;
	  exit(EXIT_FAILURE);
	}

	return ObjectIntersection(true, tn.first, tn.second, m_m);
}

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
