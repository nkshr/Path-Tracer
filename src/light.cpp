#include "light.h"
PointLight::PointLight(Vec p, Spectrum spectral_radiance){
	m_p = p;
	m_srad = spectral_radiance;
}

Ray PointLight::get_shadow_ray(const Vec &p) const {
	return Ray (p, (m_p - p).norm());
}


Vec PointLight::get_position() const {
	return m_p;
}

Spectrum PointLight::get_spectral_radiance(const Ray &shadow_ray) const {
	return m_srad;
}

SpotLight::SpotLight(Vec p, Vec d, Spectrum srad, double deg){
	m_p = p;
	m_d = d.norm();
	m_srad = srad;
	m_ang =deg_to_rad(deg);
	m_cos = cos(m_ang);
}

Ray SpotLight::get_shadow_ray(const Vec &p) const {
	return Ray(p, (m_p - p).norm());
}

Spectrum SpotLight::get_spectral_radiance(const Ray& shadow_ray) const {
	if (m_d.dot(shadow_ray.direction * -1) > m_cos) {
		return m_srad;
	}

	return Spectrum(0.0);
}

Vec SpotLight::get_position() const {
	return m_p;
}

Liser::Liser(Vec p, Spectrum srad, double w, double h, Vec target, Vec up){
	m_rect.normal = (target-p).norm();
	m_rect.up;
	m_rect.width = w;
	m_rect.height = h;
}

Ray Liser::get_shadow_ray(const Vec &p) const {
	return Ray(p, m_rect.normal);
}

Spectrum Liser::get_spectral_radiance(const Ray &shadow_ray) const {
	return m_srad;
}

Vec Liser::get_position() const {
	return m_rect.position;
}
