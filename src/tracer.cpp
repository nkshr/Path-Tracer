#include <vector>

#include "tracer.h"
#include "objects.h"

ObjectIntersection Tracer::intersect(const Ray &ray, const std::vector<Object*> &objects) {
	ObjectIntersection isct = ObjectIntersection();
	ObjectIntersection temp;
	const size_t size = objects.size();

	for (int i = 0; i<size; i++) {
		temp = objects.at((unsigned)i)->get_intersection(ray);

		if (temp.hit) {
			if (isct.u == 0 || temp.u < isct.u) isct = temp;
		}
	}
	return isct;
}

Spectrum Tracer::trace_ray(Ray ray, int depth, unsigned short * Xi) {
	ObjectIntersection isct = intersect(ray, objects);

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
		double dot_product = reflected.direction.dot(isct.n);
		if (dot_product < 0.0) {
			--i;
			dot_product = 0.0;
		}
		radiances = radiances + trace_ray(reflected, depth, Xi, objects) *dot_product;
		//if (reflected.direction.dot(isct.n) < 0) {
		//	std::cout << reflected.direction.dot(isct.n) << std::endl;
		//}
		//if (ray.direction.dot(isct.n) > 0) {
		//	std::cout << ray.direction.dot(isct.n) << std::endl;
		//}
	}

	radiances = radiances.element_wise_product(albedos);
	radiances = radiances * 2.0 / (double)config::number_of_samples_per_intersection;
	radiances = m_attenuation->attenuate(isct.u, radiances);
	return  radiances;

}

Spectrum ShadowRay::trace_ray(Ray ray, int depth, unsigned short * Xi) {
	ObjectIntersection isct = intersect(ray, objects);

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
		double dot_product = reflected.direction.dot(isct.n);
		if (dot_product < 0.0) {
			--i;
			dot_product = 0.0;
		}
		radiances = radiances + trace_ray(reflected, depth, Xi, objects) *dot_product;
	}

	radiances = radiances.element_wise_product(albedos);
	radiances = radiances * 2.0 / (double)config::number_of_samples_per_intersection;
	radiances = m_attenuation->attenuate(isct.u, radiances);
	return  radiances;

}