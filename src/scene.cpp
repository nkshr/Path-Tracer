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

//double Scene::trace_ray(const Ray &ray, int depth, int samples, unsigned short*Xi) {
//	//std::cout << "depth : " << depth << std::endl;
//    ObjectIntersection isct = intersect(ray);
//
//    // If no hit, return world colour
//    if (!isct.hit){
//      std::cout << "no hit" << std::endl;
//      exit(EXIT_FAILURE);
//      return 0.0;
//    }
//    
//    if (isct.m.get_type() == EMIT) {
//      const double emission = isct.m.sample_emission(ray.lambda);
//      //std::cout << emission << std::endl;
//      // if(emission)
//      // 	std::cout << emission << std::endl;
//      //std::cout << emission << std::endl;
//      	  if(emission < 0){
//	    std::cout << "emission : " << emission << std::endl;
//	    exit(EXIT_FAILURE);
//	  }
//
//      return emission;
//    }
//    
//    const double albedo = isct.m.sample_albedo(ray.lambda);
//    if(albedo < 0){
//      std::cout << "albedo : " << albedo << std::endl;
//      exit(EXIT_FAILURE);
//    }
//    // Russian roulette termination.
//    // If random number between 0 and 1 is > p, terminate and return hit object's emmission
//    if (++depth>MAX_DEPTH){
//		return isct.m.sample_emission(ray.lambda);
//		double rnd = erand48(Xi);
//        if (rnd<albedo*0.9) { // Multiply by 0.9 to avoid infinite loop with colours of 1.0
//        }
//        else {
//	  const double emission = isct.m.sample_emission(ray.lambda);
//	  if(emission < 0){
//	    std::cout << emission << std::endl;
//	    exit(EXIT_FAILURE);
//	  }
//            return emission;
//        }
//    }
//
//    Vec x = ray.origin + ray.direction * isct.u;
//	double radiance = 0.0;
//	for (int i = 0; i < samples; ++i) {
//		Ray reflected = isct.m.get_reflected_ray(ray, x, isct.n, Xi);
//		radiance +=  trace_ray(reflected, depth, samples, Xi) * reflected.direction.dot(isct.n);
//		if(reflected.direction.dot(isct.n) < 0){
//		  std::cout << reflected.direction.dot(isct.n) << std::endl;
//		}
//	}
//
//	radiance *= (2.0 * albedo / (double)samples);
//	if(radiance < 0){
//	  std::cout << "samples :" << samples << std::endl;
//	  std::cout << "radiance : " << radiance << std::endl;
//	  exit(EXIT_FAILURE);
//	}
//	return  radiance;
//}
//
//Spectrum Scene::trace_ray(Ray ray, int samples, unsigned short *Xi) {
//	Spectrum radiances;
//	radiances.resize(m_lambdas.size());
//	for (int i = 0; i < m_lambdas.size(); ++i) {
//		ray.lambda = m_lambdas[i];
//		double radiance = trace_ray(ray, 0, samples, Xi);
//		radiances.set_elem(i, ray.lambda, radiance);
//	}
//	return radiances;
//}

Spectrum Scene::trace_ray(Ray ray, int depth, int samples, unsigned short *Xi) {
	ObjectIntersection isct = intersect(ray);

	// If no hit, return world colour
	if (!isct.hit) {
		std::cout << "no hit" << std::endl;
		exit(EXIT_FAILURE);
		return 0.0;
	}

	if (isct.m.get_type() == EMIT) {
		return isct.m.get_spectral_emissions();
	}

	const Spectrum albedos = isct.m.get_spectral_albedos();
	if (++depth>MAX_DEPTH) {
		return isct.m.get_spectral_emissions();
	}

	Vec x = ray.origin + ray.direction * isct.u;


	Spectrum radiances(0.0);
	for (int i = 0; i < samples; ++i) {
		Ray reflected = isct.m.get_reflected_ray(ray, x, isct.n, Xi);
		radiances = radiances + trace_ray(reflected, depth, samples, Xi) * reflected.direction.dot(isct.n);
		if (reflected.direction.dot(isct.n) < 0) {
			std::cout << reflected.direction.dot(isct.n) << std::endl;
		}
	}

	radiances = radiances.element_wise_product(albedos);
	radiances = radiances * 2.0  / (double)samples;
	return  radiances;
}
