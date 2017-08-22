#include <stdlib.h>

#include "vector.h"
#include "ray.h"
#include "material.h"
#include "spectrum.h"

Material::Material(MaterialType t,  Spectrum e, Spectrum a) : m_type(t),
m_spectral_emissions(e), m_spectral_albedos(a){
}

MaterialType Material::get_type() const { return m_type; }

Ray Material::get_reflected_ray(const Ray &r, Vec &p, const Vec &n,	unsigned short *Xi) const {
	// Ideal specular reflection
	switch (m_type) {
	case SPEC:
		{
			double roughness = 0.8;
			Vec reflected = r.direction - n * 2 * n.dot(r.direction);
			reflected = Vec(
				reflected.x + (erand48(Xi) - 0.5)*roughness,
				reflected.y + (erand48(Xi) - 0.5)*roughness,
				reflected.z + (erand48(Xi) - 0.5)*roughness
			).norm();

			return Ray(p, reflected);
		}
		break;
	default:
	case DIFF: 
		{
			Vec nl = n.dot(r.direction)<0 ? n : n*-1;
			double r1 = 2 * M_PI*erand48(Xi), r2 = erand48(Xi), r2s = sqrt(r2);
			Vec w = nl, u = ((fabs(w.x)>.1 ? Vec(0, 1) : Vec(1)) % w).norm(), v = w%u;
			Vec d = (u*cos(r1)*r2s + v*sin(r1)*r2s + w*sqrt(1 - r2)).norm();

			return Ray(p, d);
		}
	}
}

Spectrum Material::get_spectral_albedos() const {
	return m_spectral_albedos;
}

Spectrum Material::get_spectral_emissions() const {
	return m_spectral_emissions;
}

void Material::set_spectral_emission(Spectrum e) {
	m_spectral_emissions = e;
}

void Material::set_type(MaterialType t) {
	m_type = t;
}
//double Material::sample_albedo(const double lambda) const {
//	return m_spectral_albedos.sample(lambda);
//}
//
//double Material::sample_emission(const double lambda) const {
//	return m_spectral_emissions.sample(lambda);
//}
