#include <iostream>
#include <fstream>
#include <cstring>
using namespace std;

#include "cie_xyz.h"

bool cie_xyz::load(const char *fname) {
	ifstream ifs(fname);
	if (!ifs.good()) {
		return false;
	}

	char buf[1024];
	int num_data = 0;
	while (!ifs.eof()) {
		ifs.getline(buf, 1024);
		num_data++;
	}

	const char * delims = ", ";
	for (int i = 0; i < num_data; ++i) {
		char * val = strtok(buf, delims);
		xs[i] = atof(val);
		val = strtok(NULL, delims);
		ys[i] = atof(val);
		val = strtok(NULL, delims);
		zs[i] = atof(val);
	}

	return true;
}

