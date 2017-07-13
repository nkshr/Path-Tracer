#include <algorithm>
#include <vector>
#include <cmath>
#include <iostream>
using namespace std;

#include "attenuation.h"
#include "color.h"

const char *fname = "../data/ciexyz31.csv";
const int num_smpls = 100;
int main(int argc, char ** argv) {
	c_XYZ_color XYZ_color;
	XYZ_color.set_fname(fname);
	XYZ_color.set_model(c_XYZ_color::EMODEL_C3W);

	if (!XYZ_color.init()) {
		cerr << "Error : Couldn't initialize c_XYZ_color." << endl;
		return 1;
	}

	XYZ_color.print_RGB_to_XYZ_conversion(cout);
	XYZ_color.print_XYZ_to_RGB_conversion(cout);
	XYZ_color.set_visible_range(380.0, 780.0);

	XYZ_color.begin();
	do {
		double lambda, x, y, z;
		XYZ_color.get_elem(lambda, x, y, z);
		cout << lambda << ", " << x << ", " << y << ", " << z << endl;
	} while (XYZ_color.next());

	const double step = 360.0 / num_smpls;
	double sum_diff = 0.0;
	double max_diff = -DBL_MAX;
	double min_diff = DBL_MAX;
	for (int i = 0; i < num_smpls; ++i) {
		double r, g, b;
		convert_hsv_to_rgb(step * (double)i, 1.0, 1.0, r, g, b);
		//r = g = b = 1.0;
		c_smpl_spect spect(r, g, b);
		spect.write(NULL);

		double _r, _g, _b;
		XYZ_color.calc_RGB(spect, _r, _g, _b);
		_r = clamp(_r);
		_g = clamp(_g);
		_b = clamp(_b);
		double diff = pow(r - _r, 2.0) + pow(g - _g, 2.0) + pow(b - _b, 2.0);
		sum_diff += diff;

		cout << r << ", " << g << ", " << b << " : " << _r << ", " << _g << ", " << _b << " : " << diff << endl;

		max_diff = diff > max_diff ? diff : max_diff;
		min_diff = diff < min_diff ? diff : min_diff;
	}

	double ave_diff = sqrt(sum_diff / (double)num_smpls);
	max_diff = sqrt(max_diff);
	min_diff = sqrt(min_diff);

	cout << "ave_diff : " << ave_diff << endl;
	cout << "max_diff : " << max_diff << endl;
	cout << "min_diff : " << min_diff << endl;
	return 0;
}