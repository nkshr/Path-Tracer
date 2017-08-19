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
	m_p = p_;
	m_m = m_;
	m_r = r_;
}

double Sphere::get_radius() { return m_r; }

Cylinder::Cylinder(Vec p_, Vec d_, double r_, double h_, Material m_){
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

	double ts[4];
	if (!(d < 0)) {
		const double sqrt_d = sqrt(d);
		const double a2_recip = 1.0 / (2.0 * a);
        double t = (-b + sqrt_d) * a2_recip;
		ts[0] = t;
		if (t>0) {
			Vec q = ray.origin + ray.direction * t;

			if (m_d.dot(q - p0) > 0 && m_d.dot(q - p1) < 0) {
			  Vec n = (m_d * q.dot(m_d) - q).norm();
			  tns.push_back(std::pair<double, Vec>(t, n));
			}
		}
		
		t = (-b - sqrt_d) * a2_recip;
		ts[1] = t;
		if (t>0) {
			Vec q = ray.origin + ray.direction * t;

			if (m_d.dot(q - p0) > 0 && m_d.dot(q - p1) < 0) {
			  Vec n = (m_d * q.dot(m_d) - q).norm();
			  tns.push_back(std::pair<double, Vec>(t, n));
			}
		}
	}

	double t  = (m_d.dot(p0 - ray.origin)) / (m_d.dot(ray.direction));
	ts[2] = t;
	double r2 = m_r * m_r;
	if (t>0) {
		Vec q = ray.origin + ray.direction * t;
		Vec qp = q - p0;
		if (qp.dot(qp) <= r2) {
		  Vec n = m_d;
		  tns.push_back(std::pair<double, Vec>(t, n));
		}
	}

	t = (m_d.dot(p1 - ray.origin)) / (m_d.dot(ray.direction)); ts[3] = t;
	if (t>0) {
		Vec q = ray.origin + ray.direction * t;
		Vec qp = q - p1;
		if (qp.dot(qp) <= r2) {
		  Vec n = m_d*(-1);
		  tns.push_back(std::pair<double, Vec>(t, n));
		}
	}

	if(!tns.size()){
	  //std::cout << "tns.size() : " << tns.size() << std::endl;
	 // exit(EXIT_FAILURE);
	 //std::cout << sqrtf(pow(ray.origin.x, 2.0) + pow(ray.origin.y , 2.0)) << " " << ray.origin.z << std::endl;
	  return ObjectIntersection(false, 0.0, Vec(), m_m);
	}

	if(tns.size() >= 3){
	  std::cout << "tns.size() : " << tns.size() << std::endl;
	  exit(EXIT_FAILURE);
	}

	std::pair<double, Vec> tn = tns[0];
	if(tns.size() == 2){
	  if(tn.first > tns[1].first){
	    tn = tns[1];
	  }
	  tn.second = tn.second * (-1.0);
	  std::cout << "tns.size() : " << tns.size() << std::endl;
	  std::cout << "first : " << tn.first << std::endl;
	  std::cout << ray.origin << std::endl;
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
	double t, b=op.dot(ray.direction), det=b*b-op.dot(op)+m_r*m_r;
	if (det<0) return ObjectIntersection(hit, distance, n, m_m); 
	else det=sqrt(det);
	distance = (t=b-det)>config::eps ? t : ((t=b+det)>config::eps ? t : 0);
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

bool Object::is_inside() const{
	return m_is_inside;
}

void Object::is_inside(bool is_inside) {
	m_is_inside = is_inside;
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