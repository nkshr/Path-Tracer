#pragma once
#include "vector.h"
#include "spectrum.h"
#include "objects.h"
#include "ray.h"
#include "geometry.h"

class Light {
protected:
public:
	virtual Ray get_shadow_ray(const Vec &p) const = 0;
	virtual Spectrum get_spectral_radiance(const Ray& shadow_ray) const = 0;
	virtual Vec get_position() const = 0;
};

class PointLight : public Light{
protected:
	Vec m_p;
	Spectrum m_srad;

public:
	PointLight(Vec p, Spectrum spectral_radiance);

	virtual Ray get_shadow_ray(const Vec &p) const;
	virtual Spectrum get_spectral_radiance(const Ray& shadow_ray) const;
	virtual Vec get_position() const;
};

class SpotLight : public Light{
private:
	Spectrum m_srad;
	double m_ang;
	double m_cos;
	Vec m_p;
	Vec m_d;
public:
	SpotLight(Vec p, Vec d, Spectrum srad, double deg);

	virtual Ray get_shadow_ray(const Vec &p) const;
	virtual Spectrum get_spectral_radiance(const Ray& shadow_ray) const;
	virtual Vec get_position() const;
};

class Liser : public Light{
private:
	geo::Rectangle m_rect;
	Spectrum m_srad;

public:
	Liser(Vec p, Spectrum srad, double w, double h, Vec target, Vec up);

	virtual Ray get_shadow_ray(const Vec &p) const;
	virtual Spectrum get_spectral_radiance(const Ray& shadow_ray) const;
	virtual Vec get_position() const;

};