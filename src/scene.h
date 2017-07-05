#ifndef SCENE_H
#define SCENE_H

#include "ray.h"
#include "vector.h"
#include "objects.h"
#include "attenuation.h"

class Scene {
private:
    std::vector<Object*> m_objects;

public:
    Scene();
    void add(Object *object);
    ObjectIntersection intersect(const Ray &ray);
	c_atten_coefs atten_coefs;
    Vec trace_ray(const Ray &ray, int depth, unsigned short*Xi);
};

#endif //SCENE_H