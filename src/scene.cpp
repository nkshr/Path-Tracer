#include <vector>
#include <utility>
#include <cmath>
#include <iostream>

#include "scene.h"
#include "objects.h"
#include "ray.h"
#include "config.h"

Scene::Scene() {

}

void Scene::add(Object *object) {
    m_objects.push_back( object );
}

ObjectIntersection Scene::intersect(const Ray &ray) {
    ObjectIntersection isct = ObjectIntersection();
    ObjectIntersection temp;
    const size_t size = m_objects.size();

    for (int i=0; i<size; i++){
        temp = m_objects.at((unsigned)i)->get_intersection(ray);

        if (temp.hit) {
            if (isct.u == 0 || temp.u < isct.u) isct = temp;
        }
    }
    return isct;
}


Spectrum Scene::trace_ray(Ray ray, int depth, unsigned short *Xi) {
	ObjectIntersection isct = intersect(ray);

	// If no hit, return world colour
	if (!isct.hit) {
		return 0.0;
	}

	if (isct.m.get_type() == EMIT) {
		return m_attenuation->attenuate(isct.u, isct.m.get_spectral_emissions());
	}

	const Spectrum albedos = isct.m.get_spectral_albedos();
	if (++depth>config::maximum_depth) {
		return m_attenuation->attenuate(isct.u, isct.m.get_spectral_emissions());
	}

	Vec x = ray.origin + ray.direction * isct.u;


	Spectrum radiances(0.0);
	for (int i = 0; i < config::number_of_samples_per_intersection; ++i) {
		Ray reflected = isct.m.get_reflected_ray(ray, x, isct.n, Xi);
		radiances = radiances + trace_ray(reflected, depth, Xi) * std::max(0.0, reflected.direction.dot(isct.n));
	}

	radiances = radiances.element_wise_product(albedos);
	radiances = radiances * 2.0  / (double)config::number_of_samples_per_intersection;
	radiances = m_attenuation->attenuate(isct.u, radiances);
	return  radiances;
}

Spectrum Vacuum::attenuate(const double dist, const Spectrum &spd) {
	return spd;
}

Spectrum Water::attenuate(const double dist, const Spectrum &spd) {
	Spectrum atten_spd;
	for (int i = 0; i < config::number_of_samples_per_spectrum; ++i) {
		atten_spd[i] = spd[i] * exp(-m_atten_coefs[i] * dist);
	}
	return  atten_spd;
}

void Water::set_absorp_coefs(const Spectrum &coefs) {
	m_absorp_coefs = coefs;
	m_atten_coefs = m_absorp_coefs + m_scat_coefs;
}

void Water::set_scat_coefs(const Spectrum &coefs) {
	m_scat_coefs = coefs;
	m_atten_coefs = m_absorp_coefs + m_scat_coefs;
}

void Scene::set_attenuation(Attenuation * attenuation) {
	m_attenuation = attenuation;
}