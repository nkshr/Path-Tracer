#include <math.h>
#include <iostream>
#include <stdio.h>

#include "vector.h"
#include "ray.h"
#include "material.h"
#include "objects.h"
#include "color.h"
//#include "../lib/fastbvh/BVH.h"


ObjectIntersection::ObjectIntersection(bool hit_, double u_, Vec n_, Material m_)
{
	hit=hit_, u=u_, n=n_, m=m_;
}


Sphere::Sphere( Vec p_, double r_, Material m_ ){
	m_sphere.position = p_;
	m_sphere.radius = r_;
	m_m = m_;
	m_is_light = false;
}

double Sphere::get_radius() { return m_r; }


// Check if ray intersects with sphere. Returns ObjectIntersection data structure
ObjectIntersection Sphere::get_intersection(const Ray &ray) {
	ObjectIntersection isct;
	isct.m = m_m;
	if (m_sphere.intersect(ray, isct.u)) {
		isct.hit = true;
		isct.n = (ray.origin + ray.direction * isct.u - m_sphere.position).norm();
	}

	return isct;
}

Cylinder::Cylinder(Vec p_, Vec d_, double r_, double h_, Material m_){
	m_p = p_;
	m_d = d_.norm();
	m_r = r_;
	m_h = h_;
	m_m = m_;
	m_is_light = false;
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

ObjectIntersection Cylinder::get_intersection(const Ray&ray) {	
	ObjectIntersection isct(false, DBL_MAX);

	double t;
	Vec dh_half = m_d * 0.5 * m_h;

	if (calcRayTubeIntersection(ray, m_p, m_d, m_r, m_h, t, isct.n)) {
		Vec isct_pt = ray.origin + ray.direction * t;
		Vec p = m_p - dh_half;
		Vec a = isct_pt - p;
		Vec b = m_d * a.dot(m_d);
		isct.u = t;
		//isct.n = (isct_pt - b - p).norm();
		isct.hit = true;
	}
	

	if (calcRayDiscIntersection(ray, m_p + dh_half, m_d, m_r, t)) {
		if (t > 0 && t < isct.u) {
			isct.u = t;
			isct.n = m_d;
			isct.hit = true;
		}
	}

	if (calcRayDiscIntersection(ray, m_p - dh_half, m_d * -1, m_r, t)) {
		if (t > 0 && t < isct.u) {
			isct.u = t;
			isct.n = m_d * -1;
			isct.hit = true;
		}
	}

	if (isct.hit)
		isct.m = m_m;

	return isct;
}

Vec Object::get_position() {
	return m_p;
}

Material Object::get_material() {
	return m_m;
}

bool Object::is_light() const{
	return m_is_light;
}

//Rectangle::Rectangle(Vec p_, Vec n_, double w_, double h_, Material m_) : 
//	Object(p_, m_), m_n(n_), m_w(w_), m_h(w_){
//}
//
//ObjectIntersection Rectangle::get_intersect(const Ray &r) {
//	const double tmp = r.direction.dot(m_n);
//	if (abs(tmp) < config::eps) {
//		
//	}
//
//	const double t = (m_p - r.origin).dot(m_n) / tmp;
//
//
//	ObjectIntersection isct;
//	return isct;
//}

Cuboid::Cuboid(Vec p_, Vec dir_, Vec up_, double w_, double h_, double depth_, Material m_){
	m_p = p_;
	m_m = m_;
	m_z_dir = dir_.norm();
	m_w = w_;
	m_h = h_;
	m_depth = depth_;
	m_x_dir = up_.cross(m_z_dir).norm();
	m_y_dir = m_z_dir.cross(m_x_dir).norm();

	const Vec xw = m_x_dir * m_w;
	const Vec yh = m_y_dir * m_h;
	const Vec zd = m_z_dir * m_depth;
	const Vec half_xw = xw * 0.5;
	const Vec half_yh = yh * 0.5;
	const Vec half_zd = zd * 0.5;
	
	const Vec v0 = m_p - half_xw + half_yh + half_zd;
	const Vec v1 = v0 + xw;
	const Vec v2 = v1 - yh;
	const Vec v3 = v2 - xw;

	const Vec v4 = v0 - zd;
	const Vec v5 = v1 - zd;
	const Vec v6 = v2 - zd;
	const Vec v7 = v3 - zd;

	//face0
	m_tris[0] = Triangle(v0, v3, v2, Vec(), Vec(), Vec(), &m_m);
	m_tris[1] = Triangle(v0, v2, v1, Vec(), Vec(), Vec(), &m_m);

	//face1
	m_tris[2] = Triangle(v2, v7, v6, Vec(), Vec(), Vec(), &m_m);
	m_tris[3] = Triangle(v2, v3, v7, Vec(), Vec(), Vec(), &m_m);
	m_tris[2] = m_tris[3] = m_tris[0];
	//face2
	m_tris[4] = Triangle(v6, v7, v4, Vec(), Vec(), Vec(), &m_m);
	m_tris[5] = Triangle(v6, v4, v5, Vec(), Vec(), Vec(), &m_m);

	//face3
	m_tris[6] = Triangle(v5, v4, v0, Vec(), Vec(), Vec(), &m_m);
	m_tris[7] = Triangle(v5, v0, v1, Vec(), Vec(), Vec(), &m_m);

	//face4
	m_tris[8] = Triangle(v7, v3, v0, Vec(), Vec(), Vec(), &m_m);
	m_tris[9] = Triangle(v7, v0, v4, Vec(), Vec(), Vec(), &m_m);

	//face5
	m_tris[10] = Triangle(v6, v5, v1, Vec(), Vec(), Vec(), &m_m);
	m_tris[11] = Triangle(v6, v1, v2, Vec(), Vec(), Vec(), &m_m);
}

static int a = 0;
static int b = 0;
ObjectIntersection Cuboid::get_intersection(const Ray &r) {
	ObjectIntersection isct;
	double tmin = DBL_MAX;
	//std::vector<Triangle> tris;
	for (int i = 0; i < 12; ++i) {
		if (m_tris[i].intersect(r, isct.u, tmin, isct.n)) {
			isct.hit = true;
			isct.m = m_m;
			tmin = isct.u;
	//		tris.push_back(m_tris[i]);
		}
	}

	//bool disp = false;
	//if (r.direction.dot(isct.n)>0) {
	//	std::cout << r.direction * isct.u + r.origin << std::endl;
	//	a++;
	//	disp = true;
	//}
	//if (isct.hit)
	//	b++;

	//if (disp) {
	//	std::cout << a  << " " << (double)b << std::endl;
	//}

	return isct;
}

Plane::Plane(Vec p, Vec n, Material m) {
	m_p = p;
	m_m = m;
	m_n = n;
}

ObjectIntersection Plane::get_intersection(const Ray &r) {
	ObjectIntersection isct;
	const double rn = r.direction.dot(m_n);
	if (abs(rn) < config::eps) {
		return isct;
	}

	isct.u = (m_p - r.origin).dot(m_n) / rn;
	isct.m = m_m;
	isct.n = m_n;
	isct.hit = true;
	
	return isct;
}

Disc::Disc(Vec p, Vec n, double r, Material m) : Plane(p, n, m) , m_r(r){

}

ObjectIntersection Disc::get_intersection(const Ray &r) {
	ObjectIntersection isct = Plane::get_intersection(r);
	if (isct.hit) {
		Vec isct_pt = r.origin + r.direction * isct.u;
		if (isct_pt.mag() > m_r) {
			isct = ObjectIntersection();
		}
	}

	return isct;
}

Vec Plane::get_normal() const {
	return m_n;
}