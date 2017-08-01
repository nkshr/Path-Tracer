#include <fstream>
#include <iostream>
#include <vector>
#include <map>
#include <iterator>
#include <algorithm>
#include <cstdlib>
#include <cfloat>
#include <list>
#include <cstring>

#include "common.h"
#include "color.h"
#include "vector.h"

XYZColor::XYZColor(const char *fname) {
	load(fname);
}

bool XYZColor::load(const char * fname) {
	std::ifstream ifs(fname, std::ifstream::binary);
	if (!ifs.good())
		return false;

	int num_samples = 0;
	
	char buf[config::buffer_size];

	while (!ifs.eof()) {
		ifs.getline(buf, config::buffer_size);
		if (buf[0] == '\0')
			break;

		num_samples++;
	}

	std::vector<std::pair<double, double> > x_cmf;
	std::vector<std::pair<double, double> > y_cmf;
	std::vector<std::pair<double, double> > z_cmf;

	x_cmf.resize(num_samples);
	y_cmf.resize(num_samples);
	z_cmf.resize(num_samples);

	ifs.clear();
	ifs.seekg(0);

	const char * delims = ", \n\t";

	for (int i = 0; i < num_samples; ++i) {
		ifs.getline(buf, config::buffer_size);
		char * val = strtok(buf, delims);
		const double lambda = atof(val);
		x_cmf[i].first = lambda;
		y_cmf[i].first = lambda;
		z_cmf[i].first = lambda;

		val = strtok(NULL, delims);
		x_cmf[i].second = atof(val);
		
		val = strtok(NULL, delims);
		y_cmf[i].second = atof(val);

		val = strtok(NULL, delims);
		z_cmf[i].second = atof(val);
	}

	double lambda = config::minimum_lambda;
	for (int i = 0; i < config::number_of_samples_per_spectrum; ++i, lambda += config::lambda_step) {
		m_x_cmf[i] = sample(x_cmf, lambda);
		m_y_cmf[i] = sample(y_cmf, lambda);
		m_z_cmf[i] = sample(z_cmf, lambda);
	}

	return true;
}

Vec XYZColor::calc_emissive_XYZ(const Spectrum &emissive) {
	Vec XYZ;
	for (int i = 0; i < config::number_of_samples_per_spectrum; ++i) {
		XYZ.x += emissive[i] * m_x_cmf[i] * config::lambda_step;
		XYZ.y += emissive[i] * m_y_cmf[i] * config::lambda_step;
		XYZ.z += emissive[i] * m_z_cmf[i] * config::lambda_step;
	}

	return XYZ;
}

Vec XYZColor::calc_XYZ(const Spectrum &object, const Spectrum &light) {
	Vec XYZ;
	double N = 0.0;
	for (int i = 0; i < config::number_of_samples_per_spectrum; ++i) {
		XYZ.x += object[i] * light[i] * m_x_cmf[i];
		XYZ.y += object[i] * light[i] * m_y_cmf[i];
		XYZ.z += object[i] * light[i] * m_z_cmf[i];
		N += light[i] * m_y_cmf[i];
	}

	const double recip_N = 1.0 / N;
	XYZ = XYZ * recip_N;
	return XYZ;
}

Vec XYZColor::convert_XYZ_to_RGB(const Vec &XYZ) {
	Vec RGB;
	RGB.x = m_RGB_to_XYZ[0] * XYZ.x + m_RGB_to_XYZ[1] * XYZ.y + m_RGB_to_XYZ[2] * XYZ.z;
	RGB.y = m_RGB_to_XYZ[3] * XYZ.x + m_RGB_to_XYZ[4] * XYZ.y + m_RGB_to_XYZ[5] * XYZ.z;
	RGB.z = m_RGB_to_XYZ[6] * XYZ.x + m_RGB_to_XYZ[7] * XYZ.z + m_RGB_to_XYZ[8] * XYZ.z;
	return RGB;
}

void XYZColor::set_RGB_color_space(const RGBColorSpace rgb) {
	switch (rgb) {
	default:
	case sRGB:
		m_rx = 0.64;
		m_ry = 0.33;

		m_gx = 0.3;
		m_gy = 0.6;

		m_bx = 0.15;
		m_by = 0.06;

		m_wx = 0.3127;
		m_wy = 0.329;

		m_wY = 1.0;
		break;
	}

	m_rz = 1.0 - m_rx - m_ry;
	m_gz = 1.0 - m_gx - m_gy;
	m_bz = 1.0 - m_bx - m_by;
	m_wz = 1.0 - m_wx - m_wy;
	m_wX = m_wY * m_wx / m_wy;
	m_wZ = m_wY * (1.0 - m_wx - m_wy) / m_wy;

	double m[] = { m_rx, m_gx, m_bx,
		m_ry, m_gy, m_by,
		m_rz, m_gz, m_bz };

	double inv_m[9];
	invMat3x3(m, inv_m);

	const double rY_over_ry = inv_m[0] * m_wX + inv_m[1] * m_wY + inv_m[2] * m_wZ;
	const double gY_over_gy = inv_m[3] * m_wX + inv_m[4] * m_wY + inv_m[5] * m_wZ;
	const double bY_over_by = inv_m[6] * m_wX + inv_m[7] * m_wY + inv_m[8] * m_wZ;

	m_RGB_to_XYZ[0] = m_rx * rY_over_ry;
	m_RGB_to_XYZ[1] = m_gx * gY_over_gy;
	m_RGB_to_XYZ[2] = m_bx * bY_over_by;

	m_RGB_to_XYZ[3] = m_ry * rY_over_ry;
	m_RGB_to_XYZ[4] = m_gy * gY_over_gy;
	m_RGB_to_XYZ[5] = m_by * bY_over_by;

	m_RGB_to_XYZ[6] = m_rz * rY_over_ry;
	m_RGB_to_XYZ[7] = m_gz * gY_over_gy;
	m_RGB_to_XYZ[8] = m_bz * bY_over_by;

	invMat3x3(m_RGB_to_XYZ, m_XYZ_to_RGB);

}