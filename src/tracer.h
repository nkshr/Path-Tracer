#pragma once
#include "ray.h"
#include "scene.h"
#include "objects.h"
#include "observer.h"
#include "light.h"
#include "attenuation.h"

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

	virtual Spectrum trace_ray(const Ray &ray, int depth, unsigned short * Xi) = 0;

public:
	void set_observer(Observer * observer);
	void set_objects(std::vector<Object*> objects);
	void set_attenuation(Attenuation * attenuation);
	void set_max_depth(int max_depth);
	void set_num_bounces(int num_bounces);
	void set_num_samples_per_point(int num_samples_per_point);
	void set_num_samples_per_pixel(int num_samples_per_pixel);
	double * trace_rays();
};

class PathTracer : public Tracer {
private:
	virtual Spectrum trace_ray(const Ray &ray, int depth, unsigned short * Xi);
};

class ShadowRayPathTracer : public Tracer{
protected:
	std::vector<Light*> m_lights;

	virtual Spectrum trace_ray(const Ray &ray, int depth, unsigned short * Xi);

public:
	void set_lights(std::vector<Light*> lights);

};