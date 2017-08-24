#include <math.h>
#include <iostream>
#include <stdio.h>

#include "vector.h"
#include "ray.h"
#include "material.h"
#include "objects.h"
#include "color.h"
#include "scene.h"
//#include "../lib/fastbvh/BVH.h"


ObjectIntersection::ObjectIntersection(bool hit_, double u_, Vec n_, Material m_)
{
	hit=hit_, u=u_, n=n_, m=m_;
}


Sphere::Sphere( Vec p_, double r_, Material m_ ){
	m_p = p_;
	m_r = r_;
	m_m = m_;
	m_is_light = false;
}

double Sphere::get_radius() { return m_r; }

Spectrum Sphere::illuminate(const Scene &scene, const Vec &p, const Vec &n, const int num_samples, unsigned short *Xi) {
	Spectrum srad(0.0);
	for (int i = 0; i < num_samples; ++i) {
		const Vec sample = generateUniformSampleInHemisphere(Xi);
		const Vec y_dir = (m_p - p).norm();

		Vec x_dir, z_dir;
		createRightHandCoordinateSystem(y_dir, x_dir, z_dir);

		Vec hit_p = m_p + x_dir * sample.x + y_dir * sample.y + z_dir * sample.z;
		Ray shadow_ray(p, (hit_p - p).norm());
		ObjectIntersection isct = scene.get_intersection(shadow_ray);
		if (isct.obj == this) {
			srad = srad + m_m.get_spectral_emissions();
		}
	}

	srad = srad * 2 * config::pi / (double)num_samples;
	return srad;
}

// Check if ray intersects with sphere. Returns ObjectIntersection data structure
ObjectIntersection Sphere::get_intersection(const Ray &ray) {
	ObjectIntersection isct;
	isct.m = m_m;
	if (calcRaySphereIntersection(ray, m_p, m_r, isct.u)) {
		isct.hit = true;
		isct.n = (ray.origin + ray.direction * isct.u - m_p).norm();
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
	ObjectIntersection isct(false, DBL_MAX, Vec(), m_m);

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
		if (t < isct.u) {
			isct.u = t;
			isct.n = m_d;
			isct.hit = true;
		}
	}

	if (calcRayDiscIntersection(ray, m_p - dh_half, m_d * -1, m_r, t)) {
		if (t < isct.u) {
			isct.u = t;
			isct.n = m_d * -1;
			isct.hit = true;
		}
	}
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

Cuboid::Cuboid(Vec p_, Vec dir_, Vec up_, double w_, double h_, double depth_, Material m_){
	m_p = p_;
	m_m = m_;
	m_w = w_;
	m_h = h_;
	m_depth = depth_;

	m_z_dir = dir_.norm() * -1;
	m_x_dir = up_.cross(m_z_dir).norm();
	m_y_dir = m_z_dir.cross(m_x_dir);

	m_xw = m_x_dir * m_w;
	m_yh = m_y_dir * m_h;
	m_zd = m_z_dir * m_depth;
	m_half_xw = m_xw * 0.5;
	m_half_yh = m_yh * 0.5;
	m_half_zd = m_zd * 0.5;
}

static int a = 0;
static int b = 0;
ObjectIntersection Cuboid::get_intersection(const Ray &r) {
	ObjectIntersection isct(false, DBL_MAX, Vec(), m_m);

	Vec p = m_p + m_half_zd;
	double t;
	if (calcRayRectangleIntersection(r, p, m_z_dir, m_y_dir, m_w, m_h, t)) {
		isct.hit = true;
		if (t < isct.u) {
			isct.u = t;
			isct.n = m_z_dir;
		}
	}

	p = m_p - m_half_zd;
	if (calcRayRectangleIntersection(r, p, m_z_dir * -1, m_y_dir, m_w, m_h, t)) {
		isct.hit = true;
		if (t < isct.u) {
			isct.u = t;
			isct.n = m_z_dir * -1;
		}
	}


	p = m_p + m_half_xw;
	if (calcRayRectangleIntersection(r, p, m_x_dir, m_y_dir, m_depth, m_h, t)) {
		isct.hit = true;
		if (t < isct.u) {
			isct.u = t;
			isct.n = m_x_dir;
		}
	}

	p = m_p - m_half_xw;
	if (calcRayRectangleIntersection(r, p, m_x_dir * -1, m_y_dir, m_depth, m_h, t)) {
		isct.hit = true;
		if (t < isct.u) {
			isct.u = t;
			isct.n = m_x_dir  * -1;
		}
	}

	p = m_p + m_half_yh;
	if (calcRayRectangleIntersection(r, p, m_y_dir, m_z_dir, m_w, m_depth, t)) {
		isct.hit = true;
		if (t < isct.u) {
			isct.u = t;
			isct.n = m_y_dir;
		}
	}

	p = m_p - m_half_yh;
	if (calcRayRectangleIntersection(r, p, m_y_dir * -1, m_z_dir, m_w, m_depth, t)) {
		isct.hit = true;
		if (t < isct.u) {
			isct.u = t;
			isct.n = m_y_dir  * -1;
		}
	}

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
