#pragma once
#include "vector.h"
#include "spectrum.h"
#include "objects.h"
#include "ray.h"

class Light {
protected:
	Vec m_p;
public:
	Light(const Vec &p);
	virtual Ray get_shadow_ray(const Vec &p);
	virtual Spectrum get_radiance(const Ray& shadow_ray);
	Vec get_position() const {
		return m_p;
	}
};



