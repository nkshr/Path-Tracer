#include <map>
#include <vector>
#include <iterator>
#include <algorithm>
#include <iostream>
#include <cfloat>
using namespace std;

#include "common.h"
#include "spectrum.h"
#include "attenuation.h"
#include "../lib/lodepng/lodepng.h"

const int num_tiles = 20;
const int w_tile = 50;
const int h_tile = 50;
const char *color_tiles_path = "color_tiles.png";
const char *atten_color_tiles_path = "attenuated_color_tiles.png";
const double dist = 10.0;

int main(int argc, char **argv) {
	vector<unsigned char> pix_buf;
	vector<unsigned char> atten_pix_buf;
	double step = 360.0 / (double)num_tiles;
	const int num_pixs_per_tile = w_tile * h_tile;
	c_atten_coefs atten_coefs;
	atten_coefs.init("../data/Pope_absorp.txt", "");
	atten_coefs.scale(100.f);
	for (int i = 0; i <= num_tiles; ++i) {
		double r, g, b;
		convert_hsv_to_rgb(step * (double)i, 1.0, 1.0, r, g, b);
		c_smpl_spect spect(r, g, b);
		double atten_coef = calc_atten_coef(atten_coefs, spect);

		cout << step * i << " : " << atten_coef << " " << r << " " << g << "  " << b << endl;
		double ar = calc_attenuated_value(atten_coef, dist, r);
		double ag = calc_attenuated_value(atten_coef, dist, g);
		double ab = calc_attenuated_value(atten_coef, dist, b);
		cout << ar << " " << ag << " " << ab << endl;
		for (int j = 0; j < num_pixs_per_tile; ++j) {
			pix_buf.push_back(toInt(r));
			pix_buf.push_back(toInt(g));
			pix_buf.push_back(toInt(b));
			pix_buf.push_back(255);

			atten_pix_buf.push_back(toInt(ar));
			atten_pix_buf.push_back(toInt(ag));
			atten_pix_buf.push_back(toInt(ab));
			atten_pix_buf.push_back(255);
		}
	}

	unsigned error = lodepng::encode(color_tiles_path, pix_buf, w_tile, h_tile * num_tiles);
	if (error) std::cout << "encoder error " << error << ": " << lodepng_error_text(error) << std::endl;

	error = lodepng::encode(atten_color_tiles_path, atten_pix_buf, w_tile, h_tile * num_tiles);
	if (error) std::cout << "encoder error " << error << ": " << lodepng_error_text(error) << std::endl;

	pix_buf.clear();
	return 0;
}