#pragma once
#include <cfloat>
#include <algorithm>
#include <cstring>
#include <iostream>
#include <vector>

#include "config.h"
#include "vector.h"
#include "ray.h"

#define DMSG DMsg(__FUNCSIG__)

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

inline double min(const double a, const double b, const double  c) {
	return std::min(std::min(a, b), c);
}

inline double sample(std::vector<std::pair<double, double> > data, const double pos) {
	if (pos < data[0].first)
		return 0.0;

	for (int i = 1; i < data.size(); ++i) {
		if (pos < data[i].first) {
			return linearInterpolation(pos, data[i-1].first, data[i].first,
				data[i-1].second, data[i].second);
		}
	}

	return  0.0;
}

class DMsg{
 private:
  char m_msg[config::buffer_size];
 public:
  DMsg(const char * msg){
    strcpy(m_msg, msg);
    std::cerr << "Entering " << m_msg << std::endl;
  }

  ~DMsg(){
    std::cerr << "Exiting " << m_msg << std::endl;
  }
};


bool write_png(const unsigned char * data,
	const int width, const int height, const char * fname);

inline void mapValues(double* const vals, const int num_vals,
	const double xmin, const double xmax, const double new_xmin, const double new_xmax) {
	for (int i = 0; i < num_vals; ++i) {
	        vals[i] = mapValue(vals[i], xmin, xmax, new_xmin, new_xmax);
	}
}

inline unsigned char * convertDouble3cToUchar4c(const double * data, const int num_data) {
	unsigned char * uc_data = new unsigned char[num_data * 4];
	const int jmax = num_data * 3;
	for (int i = 0, j = 0; j < jmax;) {
		uc_data[i++] = static_cast<unsigned char>(floor(data[j++]));
		uc_data[i++] = static_cast<unsigned char>(floor(data[j++]));
		uc_data[i++] = static_cast<unsigned char>(floor(data[j++]));
		uc_data[i++] = 255;
	}

	return uc_data;
}

inline void getMinMax(const double * vals, const int num_vals,
	double &min_val, double &max_val) {
	min_val = DBL_MAX;
	max_val = -DBL_MAX;
	for (int i = 0; i < num_vals; ++i) {
		if (min_val > vals[i])
			min_val = vals[i];

		if (max_val < vals[i])
			max_val = vals[i];
	}
}

inline bool calcRayPlaneIntersection(const Ray &r, const Vec &p, const Vec &n, double &t) {
	const double rn = r.direction.dot(n);
	if (abs(rn) < config::eps) {
		return false;
	}

	t = (p - r.origin).dot(n) / rn;

	if (t > 0)
		return true;

	return false;
}

inline bool calcRayDiscIntersection(const Ray &ray, const Vec &p, const Vec &n,
	const double radius, double &t) {
	if(!calcRayPlaneIntersection(ray, p, n, t))
		return  false;

	Vec isct_pt = ray.origin + ray.direction * t;
	if ((isct_pt - p).mag() > radius) {
		return false;
	}

	return true;
}

inline bool calcRayRectangleIntersection(const Ray &r, const Vec &p, const Vec &n,  const Vec &u,
	 const double w, const double h, double &t) {
	if(!calcRayPlaneIntersection(r, p, n, t))
		return false;

	const Vec isct = r.origin + r.direction * t;
	
	const Vec x_dir = u.cross(n).norm();
	const Vec y_dir = n.cross(x_dir).norm();

	const Vec wx = x_dir * w;
	const Vec hy = y_dir * h;
	const Vec half_wx = wx * 0.5;
	const Vec half_hy = hy * 0.5;
	
	const Vec p0 = p + half_wx + half_hy;
	const Vec p1 = p0 - hy;
	const Vec p2 = p1 - wx;
	const Vec p3 = p2 + hy;

	const Vec v0 = p1 - p0;
	const Vec v1 = p2 - p1;
	const Vec v2 = p3 - p2;
	const Vec v3 = p0 - p3;

	const Vec v4 = isct - p0;
	const Vec v5 = isct - p1;
	const Vec v6 = isct - p2;
	const Vec v7 = isct - p3;

	if (v0.dot(v4) < 0)
		return false;

	if (v1.dot(v5) < 0)
		return false;

	if (v2.dot(v6) < 0)
		return false;

	if (v3.dot(v7) < 0)
		return false;

	return true;
}

//http://mrl.nyu.edu/~dzorin/rend05/lecture2.pdf
inline bool calcRayTubeIntersection(const Ray &ray, Vec p, Vec dir,
	double r, double h, double &t) {
	const double hhalf = h * 0.5;
	const Vec p0 = p - dir * hhalf;
	const Vec p1 = p + dir * hhalf;

	const Vec x0 = ray.origin - p0;
	const Vec x1 = ray.direction - dir * ray.direction.dot(dir);
	const Vec x2 = x0 - dir * x0.dot(dir);

	const double a = x1.dot(x1);
	const double b = x1.dot(x2) * 2.0;
	const double c = x2.dot(x2) - r * r;
	const double d = b*b - 4 * a * c;

	if (!(d < 0)) {
		const double sqrt_d = sqrt(d);
		const double a2_recip = 1.0 / (2.0 * a);
		double t_temp = (-b + sqrt_d) * a2_recip;
		bool intersected = false;
		t = DBL_MAX;

		if (t_temp > 0) {
			Vec q = ray.origin + ray.direction * t_temp;
			if (dir.dot(q - p0) > 0 && dir.dot(q - p1) < 0) {
				t = t_temp;
				intersected = true;
			}
		}

		t_temp = (-b - sqrt_d) * a2_recip;
		if (t_temp > 0 && t_temp < t) {
			Vec q = ray.origin + ray.direction * t_temp;
			if (dir.dot(q - p0) > 0 && dir.dot(q - p1) < 0) {
				t = t_temp;
				intersected = true;
			}
		}

		if(intersected)
			return true;
	}

	return false;
}

inline double deg_to_rad(double deg) {
	constexpr double tmp = config::pi / 180.0;
	return tmp * deg;
}

inline double rad_to_deg(double rad) {
	constexpr double tmp = 180.0 / config::pi;
	return tmp * rad;
}