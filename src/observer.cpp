#include <fstream>
#include <iostream>

#include "ray.h"
#include "observer.h"
#include "common.h"
#include "spectrum.h"

//Observer::Observer(Vec position, Vec target, Vec up, int width, int height, Spectrum mono_eq) : m_width(width), m_height(height), m_position(position),  m_mono_eq(mono_eq){
//    m_width_recp = 1./m_width;
//    m_height_recp = 1./m_height;
//    m_ratio = (double)m_width/m_height;
//
//    m_direction = (target - m_position).norm();
//    m_x_direction = up.cross(m_direction * -1).norm();
//    m_y_direction = m_x_direction.cross(m_direction).norm();
//
//    m_x_spacing = (2.0 * m_ratio)/(double)m_width;
//    m_y_spacing = (double)2.0/(double)m_height;
//    m_x_spacing_half = m_x_spacing * 0.5;
//    m_y_spacing_half = m_y_spacing * 0.5;
//}

Observer::Observer(const Config  &config) : m_config(config){
	m_width_recp = 1. / m_config.image_width;
	m_height_recp = 1. / m_config.image_height;
	m_ratio = (double)m_config.image_width / m_config.image_height;

	m_direction = (config.target - m_position).norm();
	m_x_direction = m_config.up.cross(m_direction * -1).norm();
	m_y_direction = m_x_direction.cross(m_direction).norm();

	m_x_spacing = (2.0 * m_ratio) / (double)m_config.image_width;
	m_y_spacing = (double)2.0 / (double)m_config.image_height;
	m_x_spacing_half = m_x_spacing * 0.5;
	m_y_spacing_half = m_y_spacing * 0.5;

	m_pixel_buffer = new double[m_num_pixs * 3];
}

Observer::~Observer() {
	delete[] m_pixel_buffer;
}

int Observer::get_width() { return m_config.image_width; }
int Observer::get_height() { return m_config.image_height; }

double Observer::get_sensor_size() {
	return m_sensor_size;
}

// Returns ray from camera origin through pixel at x,y
Ray Observer::get_ray(int x, int y, bool jitter_pinhole, bool jitter_pixel, unsigned short *Xi) {
	
	if (jitter_pinhole) {
	}
	else {
	}

    double x_jitter_pixel;
    double y_jitter_pixel;

    // If jitter == true, jitter point for anti-aliasing
    if (jitter_pixel) {
        x_jitter_pixel = (erand48(Xi) * m_x_spacing) - m_x_spacing_half;
        y_jitter_pixel = (erand48(Xi) * m_y_spacing) - m_y_spacing_half;
    }
    else {
        x_jitter_pixel = 0;
        y_jitter_pixel = 0;
    }

    Vec pixel = m_position + m_direction*2;
    pixel = pixel - m_x_direction*m_ratio + m_x_direction*((x * 2 * m_ratio)*m_width_recp) + x_jitter_pixel;
    pixel = pixel + m_y_direction - m_y_direction*((y * 2.0)*m_height_recp + y_jitter_pixel);

    return Ray(m_position, (pixel-m_position).norm(), Spectrum(0.0));
}

void Observer::create_image(const Spectrum *psds) {
	for (int i = 0; i < m_num_pixs; ++i) {
		Vec rgb = convert_psd_to_rgb(psds[i]);
		rgb = rgb * m_config.exposure_time;

		double max_val = -DBL_MAX;
		double min_val = DBL_MAX;

		double tmp = max(rgb.x, rgb.y, rgb.z);
		if (m_max_pix_val < tmp) {
			m_max_pix_val = tmp;
		}

		tmp = min(rgb.x, rgb.y, rgb.z);
		if (m_min_pix_val > tmp) {
			m_min_pix_val = tmp;
		}

		const int ipb = i * 3;
		m_pixel_buffer[ipb] = rgb.x;
		m_pixel_buffer[ipb + 1] = rgb.y;
		m_pixel_buffer[ipb + 2] = rgb.z;
	}
}

void Observer::read_image(const double *& buf, int &num_pixs, double &max_val, double &min_val) {
	buf = m_pixel_buffer;
	num_pixs = m_config.image_width * m_config.image_height;
	max_val = m_max_pix_val;
	min_val = m_min_pix_val;
}

MonoCamera::MonoCamera(const Config &config) : Observer(config){
	switch (m_config.model) {
	default:
	case GT1290:
		m_mono_eq.load(config::gt1290_eq_file);
		break;
	}
}

Vec MonoCamera::convert_psd_to_rgb(const Spectrum &psd) {
	Vec rgb;
	rgb.x = m_mono_eq * psd;
	rgb.y = rgb.x;
	rgb.z = rgb.y;
	return rgb;
}

RGBCamera::RGBCamera(const Config &config) : Observer(config) {
	switch (m_config.model) {
	default:
	case  MAKO:
		m_r_eq.load(config::mako_r_eq_file);
		m_g_eq.load(config::mako_g_eq_file);
		m_b_eq.load(config::mako_b_eq_file);
		break;
	}
}

Vec RGBCamera::convert_psd_to_rgb(const Spectrum &psd) {
	Vec rgb;
	rgb.x = m_r_eq * psd;
	rgb.y = m_g_eq * psd;
	rgb.z = m_b_eq * psd;
	return rgb;
}

Eye::Eye(const Config &config) : Observer(config) {
	switch (m_config.model) {
	default:
	case CIE31:
		break;
	}
}