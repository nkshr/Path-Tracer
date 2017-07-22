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

public:
	Spectrum();
	Spectrum(const double value,
		const double min_lambda = 380.0, const double max_lambda = 780, const int num_data = 500);
	Spectrum(const char *fname);
	~Spectrum();

	void init(const double value, const double min_lambda, const double max_lambda, const int num_data);

	bool load(const char *fname);

	bool write(const char *fname)const;

	double sample(const double lambda) const;

	void resize(const int i);

	void next();

	void cur(double &lambda, double &value);

	void begin();

	bool end();

	void get_elem(const int i, double &lambda, double &value) const;
	int get_num_data() const;
	double get_min_lambda() const;
	double get_max_lambda() const;

	void set_all(const double value);

	void add(const double lambda, const double value);

	//Spectrum operator*(const double a) const;
	//Spectrum operator/(const double a) const;

	Spectrum element_wise_product(const Spectrum &spectrum) const;

	friend Spectrum operator*(const Spectrum &l, const double r);
	friend Spectrum operator/(const Spectrum &l, const double r);
	friend Spectrum operator+(const Spectrum &l, const Spectrum &a);
	friend double operator*(const Spectrum &l, const Spectrum &r);
};

//Spectrum operator+(const Spectrum &a, const Spectrum &b);
