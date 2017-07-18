#pragma once
#include <vector>
#include <list>
#include <utility>

#include "vector.h"
#include "common.h"

class Spectrum {
private:
	std::vector<std::pair<double, double> > data;
	double mean_value;
	double half_step;
	std::vector<std::pair<double, double> >::iterator it_data;

public:
	Spectrum(const double value = 0.0,
		const double min_lambda = 380.0, const double max_lambda = 780, const double  step = 1.0);
	Spectrum(const char *fname);
	~Spectrum();

	bool load(const char *fname);

	bool write(const char *fname)const;

	double sample(const double lambda) const;

	void resize(const int i);

	int get_num_elems() const;

	void next();

	void cur(double &lambda, double &value);

	void begin();

	bool end();

	void get_elem(const int i, double &lambda, double &value) const;

	void set_elem(const int i, const double lambda, const double value);

	void add(const double lambda, const double value);
};

class SparseSpectrum{
private:
	std::list<std::pair<double, double> > data;

public:
	//int get_num_elems() const;
	void get_elem(const int i, double &lambda, double &value) const;

	void add(const double lambda, const double value);
};

std::ostream &operator<<(std::ostream out, const Spectrum &spectrum);

class UniformSpectrum {
private:
	double value;
	double max_lambda;
	double min_lambda;

public:
};
