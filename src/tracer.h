#pragma once
#include "ray.h"
#include "scene.h"

class Tracer {
protected:
	ObjectIntersection intersect(const Ray &ray, const std::vector<Object*> &objects);
	Attenuation * m_attenuation;
	std::vector<Object*> m_objects;
	std::vector<Object*> m_lights;

public:
	Spectrum trace_ray(Ray ray, int depth, unsigned short * Xi);
};

class ShadowRay : public Tracer{
public:
	Spectrum trace_ray(Ray ray, int depth, unsigned short * Xi, const std::vector<Object*> &objects);
};