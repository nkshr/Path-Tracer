#ifndef SCENE_H
#define SCENE_H

#include "ray.h"
#include "vector.h"
#include "objects.h"
//#include "attenuation.h"

class Scene {
private:
    std::vector<Object*> m_objects;
	std::vector<double> m_lambdas;

public:
    Scene();
    void add(Object *object);
    ObjectIntersection intersect(const Ray &ray);
    //double trace_ray(const Ray &ray, int depth, int samples, unsigned short*Xi);
	//Spectrum trace_ray(Ray ray, int samples, unsigned short*Xi);
	Spectrum trace_ray(Ray ray, int depth, int samples, unsigned short *Xi);
};

#endif //SCENE_H