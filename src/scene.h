#ifndef SCENE_H
#define SCENE_H

#include "ray.h"
#include "vector.h"
#include "objects.h"

class Scene {
public:
	enum Model {
		VACUUM, AIR, UNDERWATER
	};

	struct Config{
		std::vector<Object*> objects;
		Model model;
		char * abosrp_coefs_file;
		char * scat_coefs_file;
		double absorp_coefs_scale;
		double scat_coefs_scale;
	};

private:
	Config m_config;
    std::vector<Object*> m_objects;

protected:
	Spectrum m_atten_coefs;

public:
	Scene(const Config &config);
    Scene(const Spectrum &atten_coefs);
    void add(Object *object);
    ObjectIntersection intersect(const Ray &ray);
	Spectrum trace_ray(Ray ray, int depth, unsigned short *Xi);
	virtual Spectrum attenuate(const double dist,  const Spectrum &spd) = 0;
};

class Vacuum : public Scene {
public:
	Vacuum(const Config &config);
	Spectrum attenuate(const double dist, const Spectrum &spd);
};

class Air : public Scene {
public:
	Air(const Config &config);
	Spectrum attenuate(const double  dist, const Spectrum &spd);
};

class Underwater :public Scene {
public:
	Underwater(const Config &config);
	Spectrum attenuate(const double dist, const Spectrum  &spd);
};
#endif //SCENE_H