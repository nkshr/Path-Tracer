#pragma once
#include "vector.h"
#include "spectrum.h"
#include "objects.h"

class Light {
protected:
	Vec m_position;
	Spectrum m_spd;

public:
	Light(const Vec &p);
	virtual void illuminate(const Vec &p) = 0;
};

class Laser : public Light {
private:
	Disc m_disc;

public:
	Laser(const Vec &p, const Vec &dir, const double r);
	void illuminate(const Vec &p);
};