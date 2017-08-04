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

	int m_num_pixs;

	double m_max_pix_val;
	double m_min_pix_val;

	double *m_pixel_buffer;
protected:
	Config m_config;

public:
    //Camera(Vec position, Vec target, Vec up, int width, int height, Spectrum mono_eq);
	Camera(const Config &config);
	~Camera();
	int get_width();
    int get_height();
    Ray get_ray(int x, int y, bool jitter, unsigned short *Xi);
	void create_image(const Spectrum * psds);
	void read(const double *& buf, int &sz, double &max_bal, double &min_val);
	virtual Vec convert_psd_to_rgb(const Spectrum &psd) = 0;
};


class MonoCamera : public Camera{
private:
	Spectrum m_mono_eq;

public:
	MonoCamera(const Config &config);
	Vec convert_psd_to_rgb(const Spectrum  &spectrum);
};

//class RGBCamera : public Camera {
//private:
//	Spectrum m_r_eq;
//	Spectrum m_g_eq;
//	Spectrum m_b_eq;
//
//public:
//	RGBCamera(const Config &config);
//	Vec convert_psd_to_rgb(const Spectrum &spectrum);
//}
#endif //CAMERA_H
