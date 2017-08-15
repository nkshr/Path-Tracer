#pragma once
#include "vector.h"
#include "spectrum.h"
#include "objects.h"

class Light {
protected:
	Spectrum m_spd;
	Vec m_p;
public:
	Light(Spectrum spd, Vec p);
	virtual Spectrum illuminate(const Vec &p) = 0;
};

class Laser : public Light {
private:
	Vec m_d;
	double m_r;

public:
	Laser(Spectrum spd, Vec p, Vec d, const double r);
	void illuminate(const Vec &p, Ray &shadow_ray, Spectrum  &spd);
};