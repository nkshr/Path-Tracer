//Except for scene.h, don't include this.
#ifndef OBJECTS_H
#define OBJECTS_H

class Object {
protected:
	Vec m_p;
	Material m_m;
	bool m_is_light;
public:
	virtual ObjectIntersection get_intersection(const Ray &r) = 0;
	virtual Spectrum illuminate(const Scene &scene, const Vec &p, const Vec &n, const int num_samples, unsigned short *Xi) { return Spectrum(0.0); };

	Vec get_position();
	Material get_material();
	bool is_light() const;
};


class Sphere : public Object {

private:
	double m_r;	// Radius

public:
	Sphere(Vec p_, double r_, Material m_);	
	double get_radius();
	
	virtual ObjectIntersection get_intersection(const Ray &r);
	Spectrum illuminate(const Scene &scene, const Vec &p, const Vec &n, const int num_samples, unsigned short *Xi);

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

class Cuboid : public Object {
private:
	double m_w;
	double m_h;
	double m_depth;

	Vec m_x_dir;
	Vec m_y_dir;
	Vec m_z_dir;
	Vec m_xw;
	Vec m_yh;
	Vec m_zd;
	Vec m_half_xw;
	Vec m_half_yh;
	Vec m_half_zd;

public:
	Cuboid(Vec p_, Vec dir_, Vec up_, double w_, double h_, double depth_, Material m_);
	virtual ObjectIntersection get_intersection(const Ray &r);
};

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
