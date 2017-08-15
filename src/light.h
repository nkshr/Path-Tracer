#pragma once
#include "vector.h"
#include "spectrum.h"
#include "objects.h"

class Light {
protected:
	Vec m_position;
	Spectrum m_spd;
	Object * m_object;

public:
	Light(Vec p, Object * object);
	virtual void illuminate(const Vec &p) = 0;
};

class Laser : public Light {
private:

public:
	Laser(Vec p, Vec dir, const double r);
	void illuminate(const Vec &p);
};