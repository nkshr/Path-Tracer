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