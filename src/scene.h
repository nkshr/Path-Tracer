#ifndef SCENE_H
#define SCENE_H

#include "ray.h"
#include "vector.h"
#include "objects.h"

class Scene {
private:
    std::vector<Object*> m_objects;
	std::vector<double> m_lambdas;
	Spectrum m_atten_coefs;

public:
    Scene(const Spectrum &atten_coefs);
    void add(Object *object);
    ObjectIntersection intersect(const Ray &ray);
	Spectrum trace_ray(Ray ray, int depth, unsigned short *Xi);
	Spectrum attenuate(const double dist,  const Spectrum &prig_spectrum);
};

#endif //SCENE_H