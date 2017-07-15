#include <vector>
#include <utility>
#include <cmath>
#include <iostream>

#include "scene.h"
#include "objects.h"

#define LAMBDA_STEP 10

Scene::Scene() {
	//atten_coefs.init("../data/Pope_absorp.txt", "");
	//atten_coefs.scale(100);
}

void Scene::add(Object *object) {
    m_objects.push_back( object );
	
	constexpr double epsilon = LAMBDA_STEP * 0.5;

	bool add = false;
	Material m = object->get_material();
	if (m.get_type() == EMIT) {
		const Spectrum albedo = m.get_albedo();
		for (int i = 0; i < albedo.get_num_elems(); ++i) {
			const double lambda = albedo[i];
			for (int j = 0; j < m_lambdas.size(); ++j) {
				if (m_lambdas[j] - epsilon < lambda && lambda < m_lambdas[j] + epsilon) {
					add = true;
					break;
				}
			}

			if (add) {
				m_lambdas.push_back(lambda);
			}
		}
	}
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

double Scene::trace_ray(const Ray &ray, int depth, unsigned short*Xi) {

    ObjectIntersection isct = intersect(ray);

    // If no hit, return world colour
    if (!isct.hit) return 0.0;

    if (isct.m.get_type() == EMIT) return isct.m.sample_emission(ray.lambda);

    double albedo = isct.m.sample_albedo(ray.lambda);

    // Russian roulette termination.
    // If random number between 0 and 1 is > p, terminate and return hit object's emmission
    double rnd = erand48(Xi);
    if (++depth>5){
        if (rnd<albedo*0.9) { // Multiply by 0.9 to avoid infinite loop with colours of 1.0
        }
        else {
            return isct.m.sample_emission(ray.lambda);
        }
    }

    Vec x = ray.origin + ray.direction * isct.u;
    Ray reflected = isct.m.get_reflected_ray(ray, x, isct.n, Xi);

    //return colour.mult( trace_ray(reflected, depth, Xi) );
    double brdf = reflected.direction.dot(isct.n);
    return  isct.m.sample_emission(ray.lambda) + trace_ray(reflected, depth, Xi) * brdf;
}
