#pragma once
#include <vector>
#include <list>
#include <utility>

#include "vector.h"
#include "common.h"

class Spectrum {
private:
	std::vector<std::pair<double, double> > m_data;
	std::vector<std::pair<double, double> >::const_iterator m_it_data;

	//double mean_value;
	//double half_step;
	double m_step;

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
	double get_min_lambda() const;
	double get_max_lambda() const;
	double get_step() const;

	void set_elem(const int i, const double lambda, const double value);
	void set_all(const double value);

	void add(const double lambda, const double value);

	Spectrum operator+(const Spectrum &spectrum) const;
	Spectrum operator*(const double a) const;
	Spectrum operator/(const double a) const;

	Spectrum element_wise_product(const Spectrum &spectrum) const;
};
