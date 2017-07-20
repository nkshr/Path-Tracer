#include <vector>
#include <utility>
#include <cmath>
#include <iostream>

#include "scene.h"
#include "objects.h"

#define LAMBDA_STEP 1.0
#define MAX_DEPTH 2

Scene::Scene() {
	//atten_coefs.init("../data/Pope_absorp.txt", "");
	//atten_coefs.scale(100);
}

void Scene::add(Object *object) {
    m_objects.push_back( object );
	
	constexpr double epsilon = LAMBDA_STEP * 0.5;

	Material m = object->get_material();
	if (m.get_type() == EMIT) {
		const Spectrum emissions = m.get_spectral_emission();
		for (int i = 0; i < emissions.get_num_elems(); ++i) {
			double lambda, emission;
			emissions.get_elem(i, lambda, emission);
			if (!(emission > 0))
				continue;
			bool add = true;
			for (int j = 0; j < m_lambdas.size(); ++j) {
				if (m_lambdas[j] - epsilon < lambda && lambda < m_lambdas[j] + epsilon) {
					add = false;
					break;
				}
			}

			if (add) {
				m_lambdas.push_back(lambda);
			}
		}
	}

	// for (int i = 0; i < m_lambdas.size(); ++i) {
	// 	std::cout << m_lambdas[i] << std::endl;
	// }
}

ObjectIntersection Scene::intersect(const Ray &ray) {
    ObjectIntersection isct = ObjectIntersection();
    ObjectIntersection temp;
    long size = m_objects.size();

    for (int i=0; i<size; i++){
        temp = m_objects.at((unsigned)i)->get_intersection(ray);

        if (temp.hit) {
            if (isct.u == 0 || temp.u < isct.u) isct = temp;
        }
    }
    return isct;
}

double Scene::trace_ray(const Ray &ray, int depth, int samples, unsigned short*Xi) {
	//std::cout << "depth : " << depth << std::endl;
    ObjectIntersection isct = intersect(ray);

    // If no hit, return world colour
    if (!isct.hit){
      std::cout << "no hit" << std::endl;
      exit(EXIT_FAILURE);
      return 0.0;
    }
    return isct.u;
    
    if (isct.m.get_type() == EMIT) {
      const double emission = isct.m.sample_emission(ray.lambda);
      // std::cout << emission << std::endl;
      // if(emission)
      // 	std::cout << emission << std::endl;
      return emission;
    }
    
    double albedo = isct.m.sample_albedo(ray.lambda);

    // Russian roulette termination.
    // If random number between 0 and 1 is > p, terminate and return hit object's emmission
    if (++depth>MAX_DEPTH){
		return isct.m.sample_emission(ray.lambda);
		double rnd = erand48(Xi);
        if (rnd<albedo*0.9) { // Multiply by 0.9 to avoid infinite loop with colours of 1.0
        }
        else {
            return isct.m.sample_emission(ray.lambda);
        }
    }

    Vec x = ray.origin + ray.direction * isct.u;
	double radiance = 0.0;
	for (int i = 0; i < samples; ++i) {
		Ray reflected = isct.m.get_reflected_ray(ray, x, isct.n, Xi);
		radiance +=  trace_ray(reflected, depth, samples, Xi) * reflected.direction.dot(isct.n);

	}

	radiance *= (2.0 * albedo / (double)samples);

	return  radiance;
}

Spectrum Scene::trace_ray(Ray ray, int samples, unsigned short *Xi) {
	Spectrum radiances;
	radiances.resize(m_lambdas.size());
	for (int i = 0; i < m_lambdas.size(); ++i) {
		ray.lambda = m_lambdas[i];
		double radiance = trace_ray(ray, 0, samples, Xi);
		radiances.set_elem(i, ray.lambda, radiance);
	}
	return radiances;
}
