#include <fstream>
#include <iostream>

#include "ray.h"
#include "observer.h"
#include "common.h"
#include "spectrum.h"

Observer::Observer(){
}

Observer::~Observer() {
}

// Returns ray from camera origin through pixel at x,y
Ray Observer::get_ray(int x, int y, bool start_jitter, bool end_jitter, unsigned short *Xi) {
    double x_end_jitter;
    double y_end_jitter;

    // If jitter == true, jitter point for anti-aliasing
    if (end_jitter) {
        x_end_jitter = (erand48(Xi) * m_x_spacing) - m_x_spacing_half;
        y_end_jitter = (erand48(Xi) * m_y_spacing) - m_y_spacing_half;
    }
    else {
        x_end_jitter = 0;
        y_end_jitter = 0;
    }

	double x_start_jitter;
	double y_start_jitter;
	if (start_jitter) {
		generateUniformRandInCircle(Xi, x_start_jitter, y_start_jitter);
	}
	else {
		x_start_jitter = 0;
		y_start_jitter = 0;
	}

	Vec start = m_position;
	start = start + m_x_direction * m_pinhole_radius * x_start_jitter;
	start = start + m_y_direction * m_pinhole_radius * y_start_jitter;

    Vec end = m_position + m_fvec;
    end = end - m_x_direction*m_ratio + m_x_direction*((x * 2 * m_ratio)*m_image_width_recp) + x_end_jitter;
    end = end + m_y_direction - m_y_direction*((y * 2.0)*m_image_height_recp + y_end_jitter);

    return Ray(start, (end-start).norm());
}

void Observer::update() {
	m_image_width_recp = 1. / m_image_width;
	m_image_height_recp = 1. / m_image_height;
	m_ratio = (double)m_image_width / m_image_height;

	//m_direction = (m_target - m_position).norm();

	m_z_direction = (m_position - m_target).norm();
	m_x_direction = m_up.cross(m_z_direction).norm();
	m_y_direction = m_x_direction.cross(m_z_direction * -1).norm();
		
	m_x_spacing = (2.0 * m_ratio) / (double)m_image_width;
	m_y_spacing = (double)2.0 / (double)m_image_height;
	m_x_spacing_half = m_x_spacing * 0.5;
	m_y_spacing_half = m_y_spacing * 0.5;

	m_num_pixels = m_image_width * m_image_height;

	m_sensor_size = m_sensor_width * m_sensor_height;
	
	m_forcal_length = m_ratio / tan(m_fov * 0.5);
	m_fvec = m_z_direction * m_forcal_length * -1;
}

void Observer::set_image_width(const int w) {
	m_image_width = w;
}

void Observer::set_image_height(const int h) {
	m_image_height = h;
}

void Observer::set_fov(const double fov) {
	m_fov = degToRad(clamp(fov, 0.1, 179.9));
}

void Observer::set_sensor_width(const double w) {
	m_sensor_width = w;
}

void Observer::set_sensor_height(const double h) {
	m_sensor_height = h;
}

void Observer::set_exposure_time(const double t) {
	m_exposure_time = t;
}

void Observer::set_iso(const double iso) {
	m_iso = iso;
}

void Observer::set_pinhole_radius(const double r) {
	m_pinhole_radius = r;
}

void Observer::set_position(const Vec &position) {
	m_position = position;
}

void Observer::set_target(const Vec &target) {
	m_target = target;
}

void Observer::set_up(const Vec &up) {
	m_up = up;
}

void Observer::set_medium(Object *medium) {
	m_medium = medium;
}

int Observer::get_image_width() {
	return m_image_width;
}

int Observer::get_image_height() {
	return m_image_height;
}

int Observer::get_num_pixels() {
	return m_num_pixels;
}

double Observer::get_fov() {
	return m_fov;
}

double Observer::get_sensor_width() {
	return m_sensor_width;
}

double Observer::get_sensor_height() {
	return m_sensor_height;
}

double Observer::get_exposure_time() {
	return m_exposure_time;
}

double Observer::get_iso() {
	return m_iso;
}

double Observer::get_pinhole_radius() {
	return m_pinhole_radius;
}

Vec Observer::get_position() {
	return m_position;
}

Vec Observer::get_target() {
	return m_target;
}

Vec Observer::get_up() {
	return m_up;
}

Object* Observer::get_medium() {
	return m_medium;
}

MonoCamera::MonoCamera(){
}

Vec MonoCamera::convert_spd_to_rgb(const Spectrum &spd) {
	Vec rgb;
	rgb.x = m_mono_eq * spd;
	rgb.y = rgb.x;
	rgb.z = rgb.y;
	return rgb;
}

void MonoCamera::set_mono_eq(const Spectrum &mono_eq) {
	m_mono_eq = mono_eq;
}

RGBCamera::RGBCamera(){
}

Vec RGBCamera::convert_spd_to_rgb(const Spectrum &spd) {
	Vec rgb;
	rgb.x = m_r_eq * spd;
	rgb.y = m_g_eq * spd;
	rgb.z = m_b_eq * spd;
	return rgb;
}

void RGBCamera::set_r_eq(const Spectrum &r_eq) {
	m_r_eq = r_eq;
}

void RGBCamera::set_g_eq(const Spectrum &g_eq) {
	m_g_eq = g_eq;
}

void RGBCamera::set_b_eq(const Spectrum &b_eq) {
	m_b_eq = b_eq;
}

Eye::Eye(){
}

Vec Eye::convert_spd_to_rgb(const Spectrum &spd) {
	Vec rgb;
	return rgb;
}

void Eye::set_XYZ_color(const XYZColor &XYZ_color) {
	m_XYZ_color = XYZ_color;
}
