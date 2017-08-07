#ifndef CAMERA_H
#define CAMERA_H

#include <vector>
#include <utility>

#include "../lib/rand48/erand48.h"
#include "vector.h"
#include "ray.h"
#include "common.h"
#include "spectrum.h"
#include "color.h"

class Observer {
public:
	enum Model {
		CIE31, GT1290, MAKO
	};

	struct Config {
		Config() : image_width(1280), image_height(960), fov(60),
			position(Vec()),  target(Vec()), up(Vec()), model(GT1290){
		}

		int image_width;
		int image_height;
		double fov;
	  double sensor_size;
	  double exposure_time;
		double iso;
		double pinhole_radius;
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

	int m_num_pixels;

	double m_max_pixel_val;
	double m_min_pixel_val;

	double *m_pixel_buffer;
protected:
	Config m_config;

public:
    //Camera(Vec position, Vec target, Vec up, int width, int height, Spectrum mono_eq);
	Observer(const Config &config);
	~Observer();
	int get_width();
    int get_height();
	double get_sensor_size();
    Ray get_ray(int x, int y, bool jitter_pinhole, bool jitter_pixel, unsigned short *Xi);
	void create_image(const Spectrum * psds);
	void read_image(const double *& buf, int &sz, double &max_bal, double &min_val);
	virtual Vec convert_psd_to_rgb(const Spectrum &psd) = 0;
};


class MonoCamera : public Observer{
private:
	Spectrum m_mono_eq;

public:
	MonoCamera(const Config &config);
	Vec convert_psd_to_rgb(const Spectrum  &spectrum);
};

class RGBCamera : public Observer {
private:
	Spectrum m_r_eq;
	Spectrum m_g_eq;
	Spectrum m_b_eq;

public:
	RGBCamera(const Config &config);
	Vec convert_psd_to_rgb(const Spectrum &psd);
};


class Eye : public Observer {
private:
	XYZColor m_XYZ_color;

public:
	Eye(const Config &config);
	Vec convert_psd_to_rgb(const Spectrum &psd);
};
#endif //CAMERA_H
