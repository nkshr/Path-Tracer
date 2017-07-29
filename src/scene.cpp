#include <vector>
#include <utility>
#include <cmath>
#include <iostream>

#include "scene.h"
#include "objects.h"
#include "ray.h"
#include "config.h"

Scene::Scene() {
	//atten_coefs.init("../data/Pope_absorp.txt", "");
	//atten_coefs.scale(100);
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


Spectrum Scene::trace_ray(Ray ray, int depth, int samples, unsigned short *Xi) {
	ObjectIntersection isct = intersect(ray);

	// If no hit, return world colour
	if (!isct.hit) {
		std::cout << "no hit" << std::endl;
		exit(EXIT_FAILURE);
		return 0.0;
	}

	if (isct.m.get_type() == EMIT) {
		return m_atten.attenuate(isct.u, isct.m.get_spectral_emissions());
	}

	const Spectrum albedos = isct.m.get_spectral_albedos();
	if (++depth>config::maximum_depth) {
		return m_atten.attenuate(isct.u, isct.m.get_spectral_emissions());
	}

	Vec x = ray.origin + ray.direction * isct.u;


	Spectrum radiances(0.0);
	for (int i = 0; i < samples; ++i) {
		Ray reflected = isct.m.get_reflected_ray(ray, x, isct.n, Xi);
		radiances = radiances + trace_ray(reflected, depth, samples, Xi) *reflected.direction.dot(isct.n);
		if (reflected.direction.dot(isct.n) < 0) {
			std::cout << reflected.direction.dot(isct.n) << std::endl;
		}
	}

	radiances = radiances.element_wise_product(albedos);
	radiances = radiances * 2.0  / (double)samples;
	radiances = m_atten.attenuate(isct.u, radiances);
	return  radiances;
}
