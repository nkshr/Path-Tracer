#pragma once
#include "common.h"

class c_XYZ_color {
public:
	enum e_model {
		EMODEL_C3W
	} model;

	c_XYZ_color();

	bool init();

	void set_fname(const char *fname);
	void set_model(const e_model &model);
	void set_visible_range(const double min_lambda, const double max_lambda);

	void calc_XYZ(c_smpl_spect &spect, double &X, double &Y, double &Z);

	void calc_RGB(c_smpl_spect &spect, double &R, double &G, double &B);

	void convert_XYZ_to_RGB(const double X, const double Y,	const double Z,
		double &r, double &g, double &b);

	void c_XYZ_color::convert_RGB_to_XYZ(const double R, const double G, const double B,
		double &X, double &Y, double &Z);

	void sample(const double lambda, double &x, double &y, double &z);

	void calc_conversion();

	void begin();

	void get_elem(double &lambda, double &x, double &y, double &z);

	bool next();	

	void print_XYZ_to_RGB_conversion(ostream &os);
	void print_RGB_to_XYZ_conversion(ostream &os);

private:
	int num_elems;

	double *lambdas;
	double *xs;
	double *ys;
	double *zs;

	double rx, ry, rz;
	double gx, gy, gz;
	double bx, by, bz;
	double wx, wy, wz;
	double wX, wY, wZ;

	double RGB_to_XYZ[9];
	double XYZ_to_RGB[9];

	double step;
	
	double min_lambda;
	double max_lambda;

	int idx;

	char fname[1024];

	bool load();

	void init_model();
};

inline void convert_hsv_to_rgb(const double h, const double s, const double v,
	double &r, double &g, double &b) {
	const double c = s * v;
	const double _h = h / 60.0;
	const double x = c * (1 - abs(fmod(_h, 2.0) - 1.0));
	if (in_range(0.0, 1.0, _h)) {
		r = c;
		g = x;
		b = 0.0;
	}
	else if (in_range(1.0, 2.0, _h)) {
		r = x;
		g = c;
		b = 0.0;
	}
	else if (in_range(2.0, 3.0, _h)) {
		r = 0.0;
		g = c;
		b = x;
	}
	else if (in_range(3.0, 4.0, _h)) {
		r = 0.0;
		g = x;
		b = c;
	}
	else if (in_range(4.0, 5.0, _h)) {
		r = x;
		g = 0.0;
		b = c;
	}
	else if (5.0 <= _h && _h <= 6.0) {
		r = c;
		g = 0.0;
		b = x;
	}
	const double m = v - c;
	r += m;
	g += m;
	b += m;
}