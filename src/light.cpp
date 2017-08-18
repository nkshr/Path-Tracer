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

SpotLight::SpotLight(Vec p, Spectrum srad, double deg, Vec d) : Light(p, srad), 
m_ang(deg_to_rad(deg)), m_d(d.norm()){
	m_cos = cos(m_ang);
}

Spectrum SpotLight::get_spectral_radiance(const Ray& shadow_ray) const {
	if (m_d.dot(shadow_ray.direction * -1) > m_cos) {
		return m_srad;
	}

	return Spectrum(0.0);
}

Liser::Liser(Vec p, Spectrum srad, double w, double h, Vec target, Vec up) : 
	Light(p, srad), m_w(w), m_h(h), m_target(target), m_up(up){

}
