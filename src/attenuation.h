#pragma once
#include "spectrum.h"

class Attenuation {
public:
	virtual Spectrum attenuate(const double dist, const Spectrum &spd) = 0;
};

class Vacuum : public Attenuation {
public:
	Vacuum() {};
	Spectrum attenuate(const double dist, const Spectrum &spd);
};

class Water : public Attenuation {
private:
	Spectrum m_atten_coefs;
	Spectrum m_absorp_coefs;
	Spectrum m_scat_coefs;

public:
	Spectrum attenuate(const double dist, const Spectrum &spd);

	void set_absorp_coefs(const Spectrum &coefs);
	void set_scat_coefs(const Spectrum &coefs);
};
