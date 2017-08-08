#ifndef SCENE_H
#define SCENE_H

#include "ray.h"
#include "vector.h"
#include "objects.h"

class Attenuation{
public:
	virtual Spectrum attenuate(const double dist, const Spectrum &spd) = 0;
};

class Vacuum : Attenuation {
public: 
	Spectrum attenuate(const double dist, const Spectrum &spd);
};

class Water : Attenuation{
private:
	Spectrum m_atten_coefs;
	Spectrum m_absorp_coefs;
	Spectrum m_scat_coefs;

public:
	Spectrum attenuate(const double dist, const Spectrum &spd);

	void set_absorp_coefs(const Spectrum &coefs);
	void set_scat_coefs(const Spectrum &coefs);
};

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