#include <iostream>
#include <cfloat>
#include <cstring>
#include <algorithm>

#include "../lib/lodepng/lodepng.h"
#include "common.h"

bool write_png(const unsigned char * data, 
	const int width ,const int height, const char * fname) {
	unsigned error = lodepng::encode(fname, data, width, height);

	if (error) {
		std::cerr << "wirte_ping error : " << error << std::endl;
		return false;
	}

	return true;
}

void mapValues(const int size, double* const data,
	const double xmin, const double xmax, const double new_xmin, const double new_xmax) {
	for (int i = 0; i < size; ++i) {
		data[i] = mapValue(data[i], xmin, xmax, new_xmin, new_xmax);
	}
}

unsigned char * convert_double_to_uchar(const int size, const double * data) {
	unsigned char * uc_data = new unsigned char[size];
	for (int i = 0; i < size; ++i) {
		if (data[i] > 255.0) {
			uc_data[i] = 255;
		}
		else if (data[i] < 0.0) {
			uc_data[i] = 0;
		}
		else {
			uc_data[i] = static_cast<int>(data[i]);
		}
	}

	return uc_data;
}
