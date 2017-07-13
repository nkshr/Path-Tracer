#include <map>
#include <vector>
#include <iterator>
#include <algorithm>
#include <iostream>
#include <cfloat>
#include <cmath>
using namespace std;

#include "../lib/lodepng/lodepng.h"

#include "common.h"
#include "spectrum.h"
#include "attenuation.h"
#include "color.h"

const int num_tiles = 20;
const int w_tile = 50;
const int h_tile = 50;
const char *color_tiles_img_name = "color_tiles.png";
const char *atten_color_tiles_img_name = "attenuated_color_tiles.png";
const char *absorp_coefs_csv_name = "../data/Pope_absorp_coefs.csv";
const double dist = 10.0;

int main(int argc, char **argv) {
	vector<unsigned char> pix_buf;
	vector<unsigned char> atten_pix_buf;
	double step = 360.0 / (double)num_tiles;
	const int num_pixs_per_tile = w_tile * h_tile;
	c_atten_coefs atten_coefs;
	atten_coefs.init(absorp_coefs_csv_name, "");
	//atten_coefs.write_atten_coefs("../data/Pope_absorp_coefs.csv");
	atten_coefs.scale(100.f);

	for (int i = 0; i <= num_tiles; ++i) {
		double r, g, b;
		convert_hsv_to_rgb(step * (double)i, 1.0, 1.0, r, g, b);
		c_smpl_spect spect(r, g, b);
		spect.normalize();
		spect.write(NULL);

		double atten_coef = atten_coefs.calc_atten_coef(spect);

		cout << step * i << " : " << atten_coef << " " << r << " " << g << "  " << b << endl;
		double ar = atten_coefs.attenuate(atten_coef, dist, r);
		double ag = atten_coefs.attenuate(atten_coef, dist, g);
		double ab = atten_coefs.attenuate(atten_coef, dist, b);
		cout << ar << " " << ag << " " << ab << endl;
		for (int j = 0; j < num_pixs_per_tile; ++j) {
			pix_buf.push_back(to_int(r));
			pix_buf.push_back(to_int(g));
			pix_buf.push_back(to_int(b));
			pix_buf.push_back(255);

			atten_pix_buf.push_back(to_int(ar));
			atten_pix_buf.push_back(to_int(ag));
			atten_pix_buf.push_back(to_int(ab));
			atten_pix_buf.push_back(255);
		}
	}

	unsigned error = lodepng::encode(color_tiles_img_name, pix_buf, w_tile, h_tile * num_tiles);
	if (error) std::cout << "encoder error " << error << ": " << lodepng_error_text(error) << std::endl;

	error = lodepng::encode(atten_color_tiles_img_name, atten_pix_buf, w_tile, h_tile * num_tiles);
	if (error) std::cout << "encoder error " << error << ": " << lodepng_error_text(error) << std::endl;

	pix_buf.clear();
	return 0;
}