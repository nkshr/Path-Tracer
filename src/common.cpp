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