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

	m_direction = (config.target - m_config.position).norm();
	m_x_direction = m_config.up.cross(m_direction * -1).norm();
	m_y_direction = m_x_direction.cross(m_direction).norm();

	m_x_spacing = (2.0 * m_ratio) / (double)m_config.image_width;
	m_y_spacing = (double)2.0 / (double)m_config.image_height;
	m_x_spacing_half = m_x_spacing * 0.5;
	m_y_spacing_half = m_y_spacing * 0.5;

	m_num_pixels = m_config.image_width * m_config.image_height;
	m_pixel_buffer = new double[m_num_pixels * 3];
}

Observer::~Observer() {
	delete[] m_pixel_buffer;
}

// Returns ray from camera origin through pixel at x,y
Ray Observer::get_ray(int x, int y, bool jitter_pixel, bool jitter_pinhole, unsigned short *Xi) {
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

	if (jitter_pinhole) {
	}
	else {
	}

    Vec pixel = m_config.position + m_direction*2;
    pixel = pixel - m_x_direction*m_ratio + m_x_direction*((x * 2 * m_ratio)*m_width_recp) + x_jitter_pixel;
    pixel = pixel + m_y_direction - m_y_direction*((y * 2.0)*m_height_recp + y_jitter_pixel);

    return Ray(m_config.position, (pixel-m_config.position).norm(), Spectrum(0.0));
}

void Observer::create_image(const Spectrum *spds) {
  DMsg dmsg("create_image");
  m_max_pixel_val = -DBL_MAX;
  m_min_pixel_val = DBL_MAX;
	for (int i = 0; i < m_num_pixels; ++i) {
		Vec rgb = convert_spd_to_rgb(spds[i]);
		rgb = rgb * m_config.exposure_time;

		double tmp = max(rgb.x, rgb.y, rgb.z);
		if (m_max_pixel_val < tmp) {
			m_max_pixel_val = tmp;
		}

		tmp = min(rgb.x, rgb.y, rgb.z);
		if (m_min_pixel_val > tmp) {
			m_min_pixel_val = tmp;
		}

		const int ipb = i * 3;
		m_pixel_buffer[ipb] = rgb.x;
		m_pixel_buffer[ipb + 1] = rgb.y;
		m_pixel_buffer[ipb + 2] = rgb.z;
	}
}

const double * Observer::read_image() {
	return m_pixel_buffer;
}

void Observer::capture(Scene &scene) {
	const double spd_scale = m_config.sensor_size / (config::number_of_samples_per_pixel * config::number_of_samples_per_point);

	// Main Loop
	#pragma omp parallel for schedule(dynamic, 1)       // OpenMP
	for (int y = 0; y<m_config.image_height; y++) {
		unsigned short Xi[3] = { (unsigned short)0, (unsigned short)0,(unsigned short)(y*y*y) };               // Stores seed for erand48
		fprintf(stderr, "\rRendering : %.2f%% ",      // Prints
			(double)y / m_config.image_height * 100);                   // progress

		for (int x = 0; x<m_config.image_width; x++) {
			Spectrum &spd = m_spds[y * m_config.image_width + x];
			for (int s1 = 0; s1 < config::number_of_samples_per_pixel; ++s1) {
				for (int s2 = 0; s2 < config::number_of_samples_per_point; ++s2) {
					Ray ray = get_ray(x, y, s1 > 0, s2 > 0, Xi);
					spd = spd + scene.trace_ray(ray, 0, Xi);
				}
			}
			spd = spd * spd_scale * m_config.sensor_size;
		}
	}

	for (int i = 0; i < m_num_pixels; ++i) {
		Vec rgb = convert_spd_to_rgb(m_spds[i]);
		rgb = rgb * m_config.exposure_time;

		const int ipb = i * 3;
		m_pixel_buffer[ipb] = rgb.x;
		m_pixel_buffer[ipb + 1] = rgb.y;
		m_pixel_buffer[ipb + 2] = rgb.z;
	}

}

MonoCamera::MonoCamera(const Config &config) : Observer(config){
	m_mono_eq.load(m_config.mono_eq_file);
}

Vec MonoCamera::convert_spd_to_rgb(const Spectrum &spd) {
	Vec rgb;
	rgb.x = m_mono_eq * spd;
	rgb.y = rgb.x;
	rgb.z = rgb.y;
	return rgb;
}

RGBCamera::RGBCamera(const Config &config) : Observer(config) {
	m_r_eq.load(m_config.r_eq_file);
	m_g_eq.load(m_config.g_eq_file);
	m_b_eq.load(m_config.b_eq_file);
}

Vec RGBCamera::convert_spd_to_rgb(const Spectrum &spd) {
	Vec rgb;
	rgb.x = m_r_eq * spd;
	rgb.y = m_g_eq * spd;
	rgb.z = m_b_eq * spd;
	return rgb;
}

Eye::Eye(const Config &config) : Observer(config) {
	m_XYZ_color.load(m_config.XYZ_cmf_file);
}

Vec Eye::convert_spd_to_rgb(const Spectrum &spd) {
	Vec rgb;
	return rgb;
}

Observer * generateObserver(Observer::Config &config) {
	switch (config.type) {
	default:
	case Observer::MONO:
		return new MonoCamera(config);
	case Observer::RGB:
		return new RGBCamera(config);
		break;
	case Observer::EYE:
		return new Eye(config);
		break;
	}
}

