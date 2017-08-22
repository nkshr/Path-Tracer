#include "light.h"
#include  "scene.h"
//PointLight::PointLight(Vec p, Spectrum spectral_radiance){
//	m_p = p;
//	m_srad = spectral_radiance;
//}
//
//Vec PointLight::get_shadow_ray_dir(const Vec &p) const {
//	return (m_p - p).norm();
//}
//
//
//Vec PointLight::get_position() const {
//	return m_p;
//}
//
//Spectrum PointLight::get_spectral_radiance(const Ray &shadow_ray) const {
//	return m_srad;
//}
//
//SpotLight::SpotLight(Vec p, Vec d, Spectrum srad, double deg){
//	m_p = p;
//	m_d = d.norm();
//	m_srad = srad;
//	m_ang =degToRad(deg);
//	m_cos = cos(m_ang);
//}
//
//Vec SpotLight::get_shadow_ray_dir(const Vec &p) const {
//	return (m_p - p).norm();
//}
//
//Spectrum SpotLight::get_spectral_radiance(const Ray& shadow_ray) const {
//	if (m_d.dot(shadow_ray.direction * -1) > m_cos) {
//		return m_srad;
//	}
//
//	return Spectrum(0.0);
//}
//
//Vec SpotLight::get_position() const {
//	return m_p;
//}
//
//Liser::Liser(Vec p, Vec target, Vec up, double w, double h, Spectrum srad){
//	m_rect.normal = (target-p).norm();
//	m_rect.up = up;
//	m_rect.width = w;
//	m_rect.height = h;
//
//	m_srad = srad;
//}
//
//Vec Liser::get_shadow_ray_dir(const Vec &p) const {
//	Ray shadow_ray(p, m_rect.normal * -1);
//	//double t;
//	//if (!m_rect.intersect(shadow_ray, t))
//	//	return Vec();
//
//	//if (t < 0)
//	//	return Vec();
//
//	return shadow_ray.direction;
//}
//
//Spectrum Liser::get_spectral_radiance(const Ray &shadow_ray) const {
//	double t;
//	if (!m_rect.intersect(shadow_ray, t)) {
//		return Spectrum(0.0);
//	}
//
//	return m_srad;
//}
//
//Vec Liser::get_position() const {
//	return m_rect.position;
//}


ObjectIntersection PointLight::get_intersection(const Ray &ray) {
	return ObjectIntersection();
}

PointLight::PointLight(Vec p, Spectrum srad) {
	m_p = p;
	m_srad = srad;
}

Spectrum PointLight::illuminate(const Scene &scene, const Vec &p, const Vec &n) {
	Ray shadow_ray(p, (m_p - p).norm());
	ObjectIntersection isct = scene.get_intersection(shadow_ray);
	const double d = (m_p - p).mag();
	if (!isct.hit || d < isct.u) {
		return m_srad * std::max(0.0, n.dot(shadow_ray.direction));
	}

	return Spectrum(0.0);
}

SpotLight::SpotLight(Vec p, Spectrum srad, double deg) {
	m_p = p;
	m_srad = srad;
	m_rad = degToRad(deg * 0.5);
}

ObjectIntersection SpotLight::get_intersection(const Ray &ray) {
	return ObjectIntersection();
}

Spectrum SpotLight::illuminate(const Scene &scene, const Vec  &p, const Vec &n) {
	Ray shadow_ray(p, (m_p - p).norm());
	double dot = n.dot(shadow_ray.direction);
	if (dot < cos(m_rad))
		return Spectrum(0.0);

	ObjectIntersection isct = scene.get_intersection(shadow_ray);
	const double d = (m_p - p).mag();
	if (!isct.hit || d < isct.u) {
		return m_srad * std::max(0.0, n.dot(shadow_ray.direction));
	}

	return Spectrum(0.0);
}