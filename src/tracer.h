#pragma once
#include "ray.h"
#include "scene.h"
#include "objects.h"
#include "observer.h"
#include "light.h"

class Tracer {
protected:
	Observer * m_observer;
	std::vector<Object*> m_objects;
	ObjectIntersection Tracer::intersect(const Ray &ray);
	Attenuation *m_attenuation;

	int m_max_depth;
	int m_num_bounces;
	int m_num_samples_per_point;
	int m_num_samples_per_pixel;

	virtual Spectrum trace_ray(Ray ray, int depth, unsigned short * Xi);

public:

	double * trace_ray();
};

class ShadowRay : public Tracer{
protected:
	std::vector<Light*> lights;

	virtual Spectrum trace_ray(Ray ray, int depth, unsigned short * Xi);

public:
};