#pragma once

#include <vector>
#include <utility>
#include <cmath>

#include "spectrum.h"
#include "config.h"
#include "vector.h"

class XYZColor {
public:
	XYZColor(const char *fname = "");
	bool load(const char * fname);
	Vec calc_emissive_XYZ(const Spectrum & spectrum);
	Vec calc_XYZ(const Spectrum &object, const Spectrum &light);
	Vec convert_XYZ_to_RGB(const Vec& XYZ);

	enum RGBColorSpace {
		sRGB
	};

	void set_RGB_color_space(const RGBColorSpace rgb);

private:
	Spectrum m_x_cmf;
	Spectrum m_y_cmf;
	Spectrum m_z_cmf;

	double m_XYZ_to_RGB[9];
	double m_RGB_to_XYZ[9];

	double m_rx, m_ry, m_rz;
	double m_gx, m_gy, m_gz;
	double m_bx, m_by, m_bz;
	double m_wx, m_wy, m_wz;
	double m_wX, m_wY, m_wZ;
};