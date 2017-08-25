#include <fstream>
#include <cstring>
#include <cassert>

#include "spectrum.h"

bool load(const char *fname, Spectrum &spectrum) {
	std::ifstream ifs(fname, std::ifstream::binary);
	if (!ifs.good()) {
		return false;
	}

	int num_samples = 0;

	char buf[config::buffer_size];

	while (!ifs.eof()) {
		ifs.getline(buf, config::buffer_size);
		if (buf[0] == '\0')
			break;

		num_samples++;
	}

	std::vector<std::pair<double, double> > samples;
	samples.resize(num_samples);

	ifs.clear();
	ifs.seekg(0);

	const char * delims = ", \n\t";
	for (int i = 0; i < num_samples; ++i) {
		ifs.getline(buf, config::buffer_size);
		char * val = strtok(buf, delims);
		samples[i].first = atof(val);

		val = strtok(NULL, delims);
		samples[i].second = atof(val);
	}

	double lambda = config::minimum_lambda;
	const double step = config::lambda_step;

	for (int i = 0; i < config::number_of_samples_per_spectrum; ++i, lambda += step) {
		spectrum[i] = sample(samples, lambda);
	}

	return true;
}