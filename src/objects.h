#ifndef OBJECTS_H
#define OBJECTS_H

#include "vector.h"
#include "ray.h"
#include "material.h"
#include "aabbox.h"
#include "triangle.h"
#include "geometry.h"
#include "../lib/tiny_obj_loader/tiny_obj_loader.h"
//#include "../lib/fastbvh/BVH.h"

struct ObjectIntersection {
	bool hit;	// If there was an intersection
	double u;	// Distance to intersection along ray
	Vec n;		// Normal of intersected face
	Material m;	// Material of intersected face

	ObjectIntersection(bool hit_=false, double u_=0, Vec n_=Vec(), Material m_=Material());
};


class Object {
protected:
	Vec m_p;
	Material m_m;
	bool m_is_light;
public:
	//Object(Vec p_, Material m_):m_p(p_), m_m(m_) {
	//}
	virtual ObjectIntersection get_intersection(const Ray &r) = 0;
	virtual Vec get_shadow_ray_dir(const Vec &p) const { return Vec(); };
	Vec get_position();
	Material get_material();
	bool is_light() const;
};


class Sphere : public Object {

private:
	double m_r;	// Radius
	geo::Sphere m_sphere;
public:
	Sphere(Vec p_, double r_, Material m_);	
	double get_radius();
	
	virtual ObjectIntersection get_intersection(const Ray &r);
	virtual Vec get_shadow_ray_dir(const Vec &p) const;
};

class Cylinder : public Object {
private:
	double m_r; //Radius
	double m_h; //Height
	Vec m_d;

public:
	Cylinder(Vec p_, Vec d_, double r_, double h_, Material m_);
	double get_radius();
	double get_height();
	Vec get_direction();
	virtual ObjectIntersection get_intersection(const Ray &r);
};

//class Rectangle : public Object {
//private:
//	Vec m_n;
//	double m_w;
//	double m_h;
//
//public:
//	Rectangle(Vec p_, Vec d_, double w_, double h_, Material m_);
//	virtual ObjectIntersection get_intersect(const Ray &r);
//};

class Cuboid : public Object {
private:
	double m_w;
	double m_h;
	double m_depth;
	Vec m_x_dir;
	Vec m_y_dir;
	Vec m_z_dir;

	Triangle m_tris[12];

public:
	Cuboid(Vec p_, Vec dir_, Vec up_, double w_, double h_, double depth_, Material m_);
	virtual ObjectIntersection get_intersection(const Ray &r);
};

//class Mesh : public Object {
//
//private:
//	std::vector<tinyobj::shape_t> m_shapes;
//    std::vector<tinyobj::material_t> m_materials;
//    std::vector<Material> materials;
//    std::vector<Triangle*> tris;
//    Material m_m;	// Material
//    KDNode *node;
//	//BVH bvh;
//
//public:
//	Mesh(Vec p_, const char* file_path, Material m_);
//	virtual ObjectIntersection get_intersection(const Ray &r);
//
//};
//

class Plane : public Object {
private:
	Vec m_n;
public:
	Plane(Vec p, Vec n, Material m);
	virtual ObjectIntersection get_intersection(const Ray &r);
	Vec get_normal() const;
};

class Disc : public Plane {
private:
	double m_r;
public:
	Disc(Vec p, Vec n, double r, Material m);
	virtual ObjectIntersection get_intersection(const Ray &r);
};
#endif // OBJECTS_H