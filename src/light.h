#pragma once
#include "vector.h"
#include "spectrum.h"
#include "objects.h"
#include "ray.h"

class Light {
protected:
	Vec m_p;
	Spectrum m_srad;

public:
	Light(Vec p, Spectrum spectral_radiance);
	virtual Ray get_shadow_ray(const Vec &p) const;
	virtual Spectrum get_spectral_radiance(const Ray& shadow_ray) const;

	Vec get_position() const;
};

typedef Light PointLight;

class SpotLight : public Light{
private:
	double m_ang;
	double m_cos;
	Vec m_d;
public:
	SpotLight(Vec p, Spectrum srad, double deg, Vec d);
	virtual Spectrum get_spectral_radiance(const Ray& shadow_ray) const;
};

class Liser : public Light{
private:
	double m_w;
	double m_h;
	Vec m_target;
	Vec m_up;
public:
	Liser(Vec p, Spectrum srad, double w, double h, Vec target, Vec up);

};