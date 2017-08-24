#include "light.h"
#include  "scene.h"

ObjectIntersection PointLight::get_intersection(const Ray &ray) {
	return ObjectIntersection();
}

PointLight::PointLight(Vec p, Spectrum srad) {
	m_p = p;
	m_srad = srad;
	m_is_light = true;
}

Spectrum PointLight::illuminate(const Scene &scene, const Vec &p, const Vec &n, const int num_samples, unsigned short *Xi) {
	Ray shadow_ray(p, (m_p - p).norm());
	shift(n, shadow_ray);

	ObjectIntersection isct = scene.get_intersection(shadow_ray);
	const double d = (m_p - p).mag();

	if (!isct.hit || d < isct.u) {
		return m_srad * std::max(0.0, n.dot(shadow_ray.direction));
	}

	return Spectrum(0.0);
}

SpotLight::SpotLight(Vec p, Vec t, Spectrum srad, double deg) {
	m_p = p;
	m_d = (t-m_p).norm();
	m_srad = srad;
	m_rad = degToRad(deg * 0.5);
	m_is_light = true;
}

ObjectIntersection SpotLight::get_intersection(const Ray &ray) {
	return ObjectIntersection();
}

Spectrum SpotLight::illuminate(const Scene &scene, const Vec  &p, const Vec &n, 
	const int num_samples, unsigned short *Xi) {
	Ray shadow_ray(p, (m_p - p).norm());
	shift(shadow_ray.direction, shadow_ray);
	double dot = m_d.dot(shadow_ray.direction) * -1;
	if (dot < cos(m_rad))
		return Spectrum(0.0);

	ObjectIntersection isct = scene.get_intersection(shadow_ray);
	const double d = (m_p - p).mag();
	if (!isct.hit || d < isct.u) {
		return m_srad * std::max(0.0, n.dot(shadow_ray.direction));
	}

	return Spectrum(0.0);
}

Laser::Laser(Vec p, Vec t, double r, double h, Spectrum srad, Material cover) {
	m_p = p;
	m_d = (t - m_p).norm();
	m_r = r;
	m_h = h;
	m_srad = srad;
	m_m = cover;
	m_is_light = true;
}

ObjectIntersection Laser::get_intersection(const Ray &r) {
	ObjectIntersection isct(false, DBL_MAX, Vec(), m_m);
	double u;
	Vec n;

	if (calcRayTubeIntersection(r, m_p, m_d, m_r, m_h, u, n)) {
		isct.hit = true;
		n = n * -1;
		isct.u = u;
		isct.n = n;
	}

	if (calcRayTubeIntersection(r, m_p, m_d, m_r+config::eps, m_h, u, n)) {
		if (u < isct.u) {
			isct.hit = true;
			isct.u = u;
			isct.n = n;
		}
	}

	Vec disc_position = m_p - m_d * (m_h * 0.5);
	if (calcRayDiscIntersection(r, disc_position, m_d, m_r, u)) {
		if (u < isct.u) {
			isct.hit = true;
			isct.n = m_d;
			isct.m.set_type(EMIT);
			isct.m.set_spectral_emission(m_srad);
		}
	}

	disc_position = m_p - m_d * (m_h * 0.5 + config::eps);
	if (calcRayDiscIntersection(r, disc_position, m_d * -1, m_r, isct.u)) {
		if (u < isct.u) {
			isct.hit = true;
			isct.n = m_d * -1;
			isct.m = m_m;
		}
	}
	return isct;
}

Spectrum Laser::illuminate(const Scene &scene, const Vec &p, const Vec &n, 
	const int num_samples, unsigned short *Xi) {
	return Spectrum(0.0);
}
