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
const char *XYZ_color_csv_name = "../data/ciexyz31.csv";

const double dist = 10.0;

int main(int argc, char **argv) {
	vector<unsigned char> pix_buf;
	vector<unsigned char> atten_pix_buf;
	double step = 360.0 / (double)num_tiles;
	const int num_pixs_per_tile = w_tile * h_tile;
	c_atten_coefs atten_coefs;
	atten_coefs.init(absorp_coefs_csv_name, "");
	atten_coefs.scale(100.f);
	
	c_XYZ_color XYZ_color;
	XYZ_color.set_fname(XYZ_color_csv_name);
	XYZ_color.set_model(c_XYZ_color::EMODEL_C3W);
	for (int i = 0; i <= num_tiles; ++i) {
		double r, g, b;
		convert_hsv_to_rgb(step * (double)i, 1.0, 1.0, r, g, b);
		c_smpl_spect spect(r, g, b);

		spect.write(NULL);
		atten_coefs.attenuate(dist, spect);
		
		char buf[1024];
		snprintf(buf, 1024, "atten_smpl_spec_%f_%f_%f.csv", r, g, b);
		spect.write(buf);


	}

	//unsigned error = lodepng::encode(color_tiles_img_name, pix_buf, w_tile, h_tile * num_tiles);
	//if (error) std::cout << "encoder error " << error << ": " << lodepng_error_text(error) << std::endl;

	//error = lodepng::encode(atten_color_tiles_img_name, atten_pix_buf, w_tile, h_tile * num_tiles);
	//if (error) std::cout << "encoder error " << error << ": " << lodepng_error_text(error) << std::endl;

	//pix_buf.clear();
	return 0;
}