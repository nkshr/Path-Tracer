#include "light.h"
#include "common.h"

Light::Light(Spectrum spd, Vec p) :  m_spd(spd), m_p(p){
}

Laser::Laser(Spectrum spd, Vec p, Vec d, const double r) : Light(spd, m_p), m_d(d), m_r(r){
	
}

void Laser::illuminate(const Vec &p, Ray &shadow_ray, Spectrum &spd) {
	shadow_ray = Ray(p, m_d * -1);

	double t;
	if (calcRayDiscIntersection(shadow_ray, m_d, m_p, m_r, t)) {
		spd = m_spd;
	}

	spd = Spectrum(0.0);
}

