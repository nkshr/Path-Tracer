#include  "scene.h"

Intersection PointLight::get_intersection(const Ray &ray) {
	return Intersection();
}

PointLight::PointLight(Vec p, Spectrum srad) {
	m_p = p;
	m_srad = srad;
	m_is_light = true;
}

Spectrum PointLight::illuminate(const Scene &scene, const Vec &p, const Vec &n, const int num_samples, unsigned short *Xi) {
	Ray shadow_ray(p, (m_p - p).norm());
	shift(n, shadow_ray);

	Intersection isct = scene.get_intersection(shadow_ray);
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

Intersection SpotLight::get_intersection(const Ray &ray) {
	return Intersection();
}

Spectrum SpotLight::illuminate(const Scene &scene, const Vec  &p, const Vec &n, 
	const int num_samples, unsigned short *Xi) {
	Ray shadow_ray(p, (m_p - p).norm());
	shift(shadow_ray.direction, shadow_ray);
	double dot = m_d.dot(shadow_ray.direction) * -1;
	if (dot < cos(m_rad))
		return Spectrum(0.0);

	Intersection isct = scene.get_intersection(shadow_ray);
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

Intersection Laser::get_intersection(const Ray &r) {
	Intersection isct(false, DBL_MAX, Vec(), m_m);
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
			isct.u = u;
			isct.m.set_type(EMIT);
			isct.m.set_spectral_emission(m_srad);
		}
	}

	disc_position = m_p - m_d * (m_h * 0.5 + config::eps);
	if (calcRayDiscIntersection(r, disc_position, m_d * -1, m_r, u)) {
		if (u < isct.u) {
			isct.hit = true;
			isct.n = m_d * -1;
			isct.u = u;
			isct.m = m_m;
		}
	}
	return isct;
}

Spectrum Laser::illuminate(const Scene &scene, const Vec &p, const Vec &n, 
	const int num_samples, unsigned short *Xi) {
	Spectrum srad(0.0);

	Vec x_dir, z_dir;
	createRightHandCoordinateSystem(m_d, x_dir, z_dir);

	for (int i = 0; i < num_samples; ++i) {
		double x, z;
		generateUniformRandInCircle(Xi, x, z);

		Vec sampled_pt = m_p + x_dir * m_r + z_dir * m_r;
		Ray shadow_ray(p, (sampled_pt - p).norm());
		Intersection isct = scene.get_intersection(shadow_ray);
		if (isct.obj == this) {
			srad = srad + m_srad;
		}
	}

	srad = srad * m_r * m_r * config::pi / (double)num_samples;
	return srad;
}
