#ifndef CAMERA_H
#define CAMERA_H

#include <vector>
#include <utility>

#include "../lib/rand48/erand48.h"
#include "vector.h"
#include "ray.h"
#include "common.h"
#include "spectrum.h"

class Camera {
public:
	enum Model {
		EYE, GT1290
	};

	struct Config {
		Config() : width(1280), height(960), fov(60),
			position(Vec()),  target(Vec()), up(Vec()), model(GT1290){
		}
		int width;
		int height;
		double fov;
		Vec position;
		Vec target;
		Vec up;
		Model model;
	};

private:
	Config m_config;
    double m_width_recp;
    double m_height_recp;
    double m_ratio;
    double m_x_spacing;
    double m_x_spacing_half;
    double m_y_spacing;
    double m_y_spacing_half;
    Vec m_position;
    Vec m_direction;
    Vec m_x_direction;
    Vec m_y_direction;
	Spectrum m_mono_eq;

public:
    //Camera(Vec position, Vec target, Vec up, int width, int height, Spectrum mono_eq);
	Camera(const Config &config);
    int get_width();
    int get_height();
	Spectrum get_mono_eq();
    Ray get_ray(int x, int y, bool jitter, unsigned short *Xi);
};

#endif //CAMERA_H
