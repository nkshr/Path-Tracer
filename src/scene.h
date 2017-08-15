#ifndef SCENE_H
#define SCENE_H

#include "ray.h"
#include "vector.h"
#include "objects.h"
#include "attenuation.h"


class Scene {
private:
    std::vector<Object*> m_objects;
	Attenuation  * m_attenuation;

protected:

public:
	Scene();
    void add(Object *object);
    ObjectIntersection intersect(const Ray &ray);
	Spectrum trace_ray(Ray ray, int depth, unsigned short *Xi);

	void set_attenuation(Attenuation * attenuation);
};

#endif //SCENE_H