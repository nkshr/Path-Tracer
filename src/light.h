#pragma once
#include "vector.h"
#include "spectrum.h"
#include "objects.h"
#include "ray.h"
#include "geometry.h"

//class Light {
//protected:
//public:
//	virtual Vec get_shadow_ray_dir(const Vec &p) const = 0;
//	virtual Spectrum get_spectral_radiance(const Ray& shadow_ray) const = 0;
//	virtual Vec get_position() const = 0;
//};
//
//class PointLight : public Light{
//protected:
//	Vec m_p;
//	Spectrum m_srad;
//
//public:
//	PointLight(Vec p, Spectrum spectral_radiance);
//
//	virtual Vec get_shadow_ray_dir(const Vec &p) const;
//	virtual Spectrum get_spectral_radiance(const Ray& shadow_ray) const;
//	virtual Vec get_position() const;
//};
//
//class SpotLight : public Light{
//private:
//	Spectrum m_srad;
//	double m_ang;
//	double m_cos;
//	Vec m_p;
//	Vec m_d;
//public:
//	SpotLight(Vec p, Vec d, Spectrum srad, double deg);
//
//	virtual Vec get_shadow_ray_dir(const Vec &p) const;
//	virtual Spectrum get_spectral_radiance(const Ray& shadow_ray) const;
//	virtual Vec get_position() const;
//};
//
//class Liser : public Light{
//private:
//	geo::Rectangle m_rect;
//	Spectrum m_srad;
//
//public:
//	Liser(Vec p, Vec target, Vec up, double w, double h, Spectrum srad);
//
//	virtual Vec get_shadow_ray_dir(const Vec &p) const;
//	virtual Spectrum get_spectral_radiance(const Ray& shadow_ray) const;
//	virtual Vec get_position() const;
//};

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
public:
	SpotLight(Vec p, Spectrum srad, double deg);
	virtual ObjectIntersection get_intersection(const Ray &r);
	Spectrum illuminate(const Scene &scene, const Vec &p, const Vec &n);
};

//class Laser : public Object {
//private:
//	Spectrum m_srad;
//
//public:
//	SpotLight(Vec p, Spectrum srad, double deg);
//	virtual ObjectIntersection get_intersection(const Ray &r);
//	Spectrum illuminate(const Scene &scene, const Vec &p, const Vec &n);
//};