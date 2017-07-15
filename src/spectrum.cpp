#include <fstream>

#include "spectrum.h"

#define BUF_SZ 1024

Spectrum::Spectrum(const double value) : lambdas(NULL), values(NULL),
mean_value(value), num_elems(0){
}

bool Spectrum::load(const char *fname) {
	std::ifstream ifs(fname, std::ifstream::binary);
	if (!ifs.good()) {
		return false;
	}

	if (num_elems) {
		delete lambdas;
		delete values;
	}

	num_elems = 0;

	char buf[BUF_SZ];

	while (!ifs.eof()) {
		ifs.getline(buf, BUF_SZ);
		if (buf[0] == '\0')
			break;

		num_elems++;
	}


	lambdas = new double[num_elems];
	values = new double[num_elems];

	ifs.clear();
	ifs.seekg(0);

	const char * delims = ", \n\t";

	for (int i = 0; i < num_elems; ++i) {
		ifs.getline(buf, BUF_SZ);
		char * val = strtok(buf, delims);
		lambdas[i] = atof(val);

		val = strtok(NULL, delims);
		values[i] = atof(val);
	}

	return true;
}

Spectrum::~Spectrum() {
	delete lambdas;
	delete values;
}

double Spectrum::sample(const double lambda) const{
	if (!num_elems)
		return mean_value;

	if (lambda < lambdas[0]) {
		return values[0];
	}

	for (int i = 0; i < num_elems; ++i) {
		if (lambdas[i] > lambda) {
			return linearInterpolation(lambda, lambdas[i - 1], lambdas[i], values[i - 1], values[i]);
		}
	}

	return values[num_elems-1];
}

Spectrum::Spectrum(const Spectrum& spectrum) {
	const int sz = sizeof(double) * spectrum.num_elems;
	lambdas = new double[spectrum.num_elems];
	memcpy((void*)lambdas, (void*)spectrum.lambdas, sz);

	values = new double[spectrum.num_elems];
	memcpy((void*)values, (void*)spectrum.values, sz);

	mean_value = spectrum.mean_value;
	num_elems = spectrum.num_elems;
}

Spectrum& Spectrum::operator=(const Spectrum &spectrum) {
	if (this == &spectrum)
		return *this;

	if (num_elems) {
		delete lambdas;
		delete values;
	}

	const int sz = sizeof(double) * spectrum.num_elems;
	lambdas = new double[spectrum.num_elems];
	memcpy((void*)lambdas, (void*)spectrum.lambdas, sz);

	values = new double[spectrum.num_elems];
	memcpy((void*)values, (void*)spectrum.values, sz);

	mean_value = spectrum.mean_value;
	num_elems = spectrum.num_elems;

	return *this;
}

double Spectrum::operator[](const int i) const{
	return lambdas[i];
}

int Spectrum::get_num_elems() const {
	return num_elems;
}
