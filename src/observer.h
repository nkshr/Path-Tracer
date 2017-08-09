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
#include "scene.h"

class Observer {
private:
    double m_ratio;
    double m_x_spacing;
    double m_x_spacing_half;
    double m_y_spacing;
    double m_y_spacing_half;

    Vec m_direction;
    Vec m_x_direction;
    Vec m_y_direction;

	int m_num_pixels;

	double *m_pixel_buffer;
private:
	int m_image_width;
	int m_image_height;
	double m_fov;
	double m_sensor_width;
	double m_sensor_height;
	double m_exposure_time;
	double m_iso;
	double m_pinhole_radius;
	Vec m_position;
	Vec m_target;
	Vec m_up;

	double m_image_width_recp;
	double m_image_height_recp;
	double m_sensor_size;

protected:
	//Config m_config;
	Spectrum * m_spds;

public:
    //Camera(Vec position, Vec target, Vec up, int width, int height, Spectrum mono_eq);
	Observer();
	~Observer();

	Ray get_ray(int x, int y, bool jitter_pixel, bool jitter_pinhole, unsigned short *Xi);
	const double * read_image();
	double * copy_image();

	virtual Vec convert_spd_to_rgb(const Spectrum &spd) = 0;
	void capture(Scene &scene);
	void update();

	void set_image_width(const int w);
	void set_image_height(const int h);
	void set_fov(const double fov);
	void set_sensor_width(const double w);
	void set_sensor_height(const double h);
	void set_exposure_time(const double t);
	void set_iso(const double iso);
	void set_pinhole_radius(const double r);
	void set_position(const Vec &position);
	void set_target(const Vec &target);
	void set_up(const Vec &up);

	int get_image_width();
	int get_image_height();
	int get_num_pixels();
	double get_fov();
	double get_sensor_width();
	double get_sensor_height();
	double get_exposure_time();
	double get_iso();
	double get_pinhole_radius();
	Vec get_position();
	Vec get_target();
	Vec get_up();
};


class MonoCamera : public Observer{
private:
	Spectrum m_mono_eq;

public:
	MonoCamera();
	Vec convert_spd_to_rgb(const Spectrum  &spectrum);

	void set_mono_eq(const Spectrum &mono_eq);
};

class RGBCamera : public Observer {
private:
	Spectrum m_r_eq;
	Spectrum m_g_eq;
	Spectrum m_b_eq;

public:
	RGBCamera();
	Vec convert_spd_to_rgb(const Spectrum &spd);

	void set_r_eq(const Spectrum &r_eq);
	void set_g_eq(const Spectrum &g_eq);
	void set_b_eq(const Spectrum &b_eq);
};


class Eye : public Observer {
private:
	XYZColor m_XYZ_color;

public:
	Eye();
	Vec convert_spd_to_rgb(const Spectrum &spd);

	void set_XYZ_color(const XYZColor &XYZ_color);
};

//Observer * generateObserver(Observer::Config &config);

#endif //CAMERA_H
