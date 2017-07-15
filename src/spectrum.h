#pragma once
#include <vector>
#include <utility>

#include "vector.h"
#include "common.h"

class Spectrum {
public:
	enum OutOfRangeType {
		ZERO, BORDER
	};
private:
	double *lambdas;
	double *values;
	double mean_value;
	int num_elems;
public:
	Spectrum(const double value = 0.0);
	Spectrum(const Spectrum &spectrum);
	~Spectrum();
	Spectrum& operator=(const Spectrum &spectrum);
	double operator[](const int i) const;

	bool load(const char *fname);

	double sample(const double lambda) const;

	int get_num_elems() const;

};

