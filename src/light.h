#pragma once
#include "vector.h"
#include "spectrum.h"
#include "objects.h"
#include "ray.h"
#include "geometry.h"

class PointLight : public Object {
private:
	Spectrum m_srad;
public:
	PointLight(Vec p, Spectrum srad);
	virtual ObjectIntersection get_intersection(const Ray &r);
	Spectrum illuminate(const Scene &scene, const Vec &p, const Vec &n);
};

class  SpotLight : public Object {
private:
	Spectrum m_srad;
	double m_rad;
	Vec m_d;

public:
	SpotLight(Vec p, Vec t, Spectrum srad, double deg);
	virtual ObjectIntersection get_intersection(const Ray &r);
	Spectrum illuminate(const Scene &scene, const Vec &p, const Vec &n);
};

class Laser : public Object {
private:
	double m_r;
	double m_w;
	double m_h;
	Spectrum m_srad;
	Vec m_d;

public:
	Laser(Vec p, Vec t, double r, double h, Spectrum srad, Material cover);
	virtual ObjectIntersection get_intersection(const Ray &r);
	virtual Spectrum illuminate(const Scene &scene, const Vec &p, const Vec &n);
};