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

TubeLight::TubeLight(Vec p, Vec t, double r, double h, Spectrum srad, Material cover) {
	m_m = cover;

	m_inner_tube.position = p;
	m_inner_tube.direction = (t - p).norm();
	m_inner_tube.radius = r;
	m_inner_tube.height = h;
	m_inner_tube.material = &m_m;
	
	m_outer_tube.position = p;
	m_outer_tube.direction = (t - p).norm();
	m_outer_tube.radius = r + config::eps;
	m_outer_tube.height = h;
	m_outer_tube.material = &m_m;

	m_bottom_disc.position = p - m_outer_tube.direction * h * 0.5;
	m_bottom_disc.normal = m_outer_tube.direction;
	m_bottom_disc.radius = r;
	m_bottom_disc.material = &m_m;

	m_is_light = true;
}

Intersection TubeLight::get_intersection(const Ray &r) {
	Intersection isct(false, DBL_MAX, Vec(), m_m);
	Vec n;
	double u;

	if (m_inner_tube.intersect(r, n, u)) {
		isct.hit = true;
		isct.n = n * -1;
		isct.u = u;
		isct.g = &m_inner_tube;
		isct.m = *m_inner_tube.material;
	}

	if (m_outer_tube.intersect(r, n, u)) {
		if (u < isct.u) {
			isct.hit = true;
			isct.n = n;
			isct.u = u;
			isct.g = &m_outer_tube;
			isct.m = *m_outer_tube.material;
		}
	}

	if (m_bottom_disc.intersect(r, n, u)) {
		if (u < isct.u) {
			isct.hit = true;
			isct.n = n;
			isct.u = u;
			isct.g = &m_bottom_disc;
			isct.m = *m_bottom_disc.material;
		}
	}

	if (m_light_source.intersect(r, n, u)) {
		if (u < isct.u) {
			isct.hit = true;
			isct.n = n;
			isct.u = u;
			isct.g = &m_light_source;
			isct.m = *m_light_source.material;
		}
	}

	return isct;
}

Spectrum TubeLight::illuminate(const Scene &scene, const Vec &p, const Vec &n, 
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
		if (isct.hit && isct.g == &m_light_source) {
			srad = srad + m_srad;
		}
	}

	srad = srad * m_r * m_r * config::pi / (double)num_samples;
	return srad;
}

Laser::Laser(Vec p, Vec t, double r, double h, Spectrum srad, Material cover) {
	m_m = cover;

	m_tube.position = p;
	m_tube.direction = (t - p).norm();
	m_tube.height = h;
	m_tube.radius = r;
	m_tube.material = &m_m;

	m_top_disc.position = p + m_tube.direction * h * 0.5;
	m_top_disc.normal = m_tube.direction;
	m_top_disc.radius = r;
	m_top_disc.material = &m_m;

	m_bottom_disc.position = p - (t - p).norm() * h * (0.5);
	m_bottom_disc.normal = m_tube.direction * -1;
	m_bottom_disc.radius = r;
	m_bottom_disc.material = &m_m;
}

Intersection Laser::get_intersection(const Ray &r) {
	Intersection isct(false, DBL_MAX, Vec(), m_m);
	Vec n;
	double u;

	if (m_tube.intersect(r, n, u)){
		isct.hit = true;
		isct.n = n;
		isct.u = u;
		isct.g = &m_tube;
		isct.m = *m_tube.material;
		m_m.set_spectral_emission(m_srad);
	}

	if (m_top_disc.intersect(r, n, u)) {
		if (u < isct.u) {
			isct.hit = true;
			isct.n = n;
			isct.u = u;
			isct.g = &m_top_disc;
			isct.m = *m_top_disc.material;
		}
	}

	if (m_bottom_disc.intersect(r, n, u)) {
		if (u < isct.u) {
			isct.hit = true;
			isct.n = n;
			isct.u = u;
			isct.g = &m_bottom_disc;
			isct.m = *m_bottom_disc.material;
		}
	}

	return isct;
}

Spectrum Laser::illuminate(const Scene &scene, const Vec &p, const Vec &n,
	const int num_samples, unsigned short *Xi) {
	Ray shadow_ray(p, m_top_disc.normal);
	Intersection isct = scene.get_intersection(shadow_ray);
	if (isct.g = &m_top_disc) {
		return m_srad;
	}

	return Spectrum(0.0);
}