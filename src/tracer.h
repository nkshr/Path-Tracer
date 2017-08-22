#pragma once
#include "ray.h"
#include "scene.h"
#include "objects.h"
#include "observer.h"
#include "light.h"
#include "attenuation.h"

class Tracer {
protected:
	Scene m_scene;
	//ObjectIntersection Tracer::intersect(const Ray &ray);
	Attenuation *m_attenuation;

	int m_max_depth;
	int m_num_bounces;
	int m_num_samples_per_pinhole;
	int m_num_samples_per_pixel;

	virtual Spectrum trace_ray(const Ray &ray, int depth, unsigned short * Xi) = 0;
	
public:
	void set_scene(Scene scene);
	void set_attenuation(Attenuation * attenuation);
	void set_max_depth(int max_depth);
	void set_num_bounces(int num_bounces);
	void set_num_samples_per_pinhole(int num_samples_per_point);
	void set_num_samples_per_pixel(int num_samples_per_pixel);
	double * trace_rays();
};

class PathTracer : public Tracer {
private:
	virtual Spectrum trace_ray(const Ray &ray, int depth, unsigned short * Xi);
};

class ShadowRayPathTracer : public Tracer{
protected:

	virtual Spectrum trace_ray(const Ray &ray, int depth, unsigned short * Xi);
	//bool visible(Light * light);
};