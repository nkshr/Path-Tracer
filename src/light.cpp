#include "light.h"

Light::Light(Vec p, Spectrum spectral_radiance) : m_p(p), m_srad(spectral_radiance){
}

Ray Light::get_shadow_ray(const Vec &p) const {
	return Ray (p, (m_p - p).norm());
}


Vec Light::get_position() const {
	return m_p;
}

Spectrum Light::get_spectral_radiance(const Ray &shadow_ray) const {
	return m_srad;
}
