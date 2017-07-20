#pragma once
#include <cfloat>
inline double linearInterpolation(const double  x, const double x1, const double  x2,
	const double y1, const double y2) {
	const double a = (x - x1) / (x2 - x1);
	return a * (y2 - y1) + y1;
}

inline bool in_range(const double &min, const double &max, const double &val) {
	if (min <= val && val < max) {
		return true;
	}
	return false;
}

inline void convert_rgb_to_hsv(const double r, const double g, const double b,
	double &h, double &s, double &v) {
	const double max_val = std::max(r, std::max(g, b));
	const double min_val = std::min(r, std::min(g, b));
	double c = max_val - min_val;

	h = 0.0;
	if (abs(c) < DBL_EPSILON) {
	}
	else if (max_val == r) {
		h = fmod((g - b) / c, 6.0);
	}
	else if (max_val == g) {
		h = (b - r) / c + 2;
	}
	else {
		h = (r - g) / c + 4;
	}

	h = 60 * h;
	if (h < 0)
		h += 360;

	v = max_val;

	if (abs(v) < DBL_EPSILON)
		s = 0.0;
	else
		s = c / v;
}

// Clamp double to min/max of 0/1
inline double clamp(double x) { return x<0 ? 0 : x>1 ? 1 : x; }
// Clamp to between 0-255
inline int toInt(double x) { return int(clamp(x) * 255 + .5); }

inline void invMat3x3(const double* m, double* inv_m) {
	//m = [m0 m1 m2
	//     m3 m4 m5
	//     m6 m7 m8]

	const double recip_det = 1.0 / (m[0] * m[4] * m[8]
		+ m[1] * m[5] * m[6]
		+ m[2] * m[3] * m[7]
		- m[2] * m[4] * m[6]
		- m[1] * m[3] * m[8]
		- m[0] * m[5] * m[7]);
	

	inv_m[0] = (m[4] * m[8] - m[5] * m[7]) * recip_det;
	inv_m[1] = (m[2] * m[7] - m[1] * m[8]) * recip_det;
	inv_m[2] = (m[1] * m[5] - m[2] * m[4]) * recip_det;

	inv_m[3] = (m[5] * m[6] - m[3] * m[8]) * recip_det;
	inv_m[4] = (m[0] * m[8] - m[2] * m[6]) * recip_det;
	inv_m[5] = (m[2] * m[3] - m[0] * m[5]) * recip_det;

	inv_m[6] = (m[3] * m[7] - m[4] * m[6]) * recip_det;
	inv_m[7] = (m[1] * m[6] - m[0] * m[7]) * recip_det;
	inv_m[8] = (m[0] * m[4] - m[1] * m[3]) * recip_det;
}

inline double mapValue(const double x, const double xmin, const double xmax, const double new_xmin, const double new_xmax) {
	const double diff = xmax - xmin;
	const double new_diff = new_xmax - new_xmin;
	double ret =  ((x - xmin)/ diff) * new_diff + new_xmin;
	return  ((x - xmin)/ diff) * new_diff + new_xmin;
}

inline double max(const double a, const double b, const double c) {
	return std::max(std::max(a, b), c);
}
