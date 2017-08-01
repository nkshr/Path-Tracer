#pragma once
#include <vector>
#include <list>
#include <utility>
#include <iostream>
#include <fstream>
#include <cstring>

#include "vector.h"
#include "common.h"
#include "config.h"

class Spectrum {
private:
	double m_samples[config::number_of_samples_per_spectrum];

public:
	Spectrum(const double val = 0.0) {
		for (int i = 0; i < config::number_of_samples_per_spectrum; ++i) {
			m_samples[i] = val;
		}
	}

	Spectrum(const char *fname) {
		if (!load(fname))
			*this = Spectrum(0.0);
	}

	Spectrum operator+(const Spectrum &r) {
		Spectrum ret;
		for (int i = 0; i < config::number_of_samples_per_spectrum; ++i) {
			ret.m_samples[i] = this->m_samples[i] +r.m_samples[i];
		}
		return ret;
	}

	double operator*(const Spectrum &r) {
		double ret = 0.0;
		for (int i = 0; i < config::number_of_samples_per_spectrum; ++i) {
			ret += this->m_samples[i] * r.m_samples[i];
		}
		return ret;
	}
	
	Spectrum operator*(const double r) {
		Spectrum ret;       
		for (int i = 0; i < config::number_of_samples_per_spectrum; ++i) {
			ret.m_samples[i] = this->m_samples[i] * r;
		}
		return ret;
	}
	Spectrum operator/(const double r) {
		Spectrum ret;
		const double r_recip = 1.0 / r;
		for (int i = 0; i < config::number_of_samples_per_spectrum; ++i) {
			ret.m_samples[i] = this->m_samples[i] * r_recip;
		}
		return ret;
	}
	
	double& operator[](const int i) {
		return m_samples[i];
	}

	double operator[](const int i) const{
		return m_samples[i];
	}
	
	friend std::ostream& operator<<(std::ostream &l, const Spectrum &r) {
		for (int i = 0; i < config::number_of_samples_per_spectrum; ++i) {
			l << config::minimum_lambda + r.get_step() * i <<", " << r.m_samples[i] << std::endl;
		}
		return l;
	}

	bool load(const char *fname) {
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
		const double step = get_step();
		for (int i = 0; i < config::number_of_samples_per_spectrum; ++i, lambda += step) {
			m_samples[i] = sample(samples, lambda);
			//std::cout << m_samples[i] << std::endl;
		}

		return true;
	}

	bool write(const char *fname) {
		std::ofstream ofs(fname);
		if (!ofs.good())
			return false;
		for (int i = 0; i < config::number_of_samples_per_spectrum; ++i) {
			ofs << i * get_step() + config::minimum_lambda << "," << m_samples[i] << std::endl;
		}
		return true;
	}

	Spectrum element_wise_product(const Spectrum &r) {
		Spectrum ret;
		for (int i = 0; i < config::number_of_samples_per_spectrum; ++i) {
			ret.m_samples[i] = this->m_samples[i] * r.m_samples[i];
		}
		return ret;
	}

	static int get_num_samples(){
		return config::number_of_samples_per_spectrum;
	}

	static double get_min_lambda(){
		return config::minimum_lambda;
	}

	static double get_max_lambda(){
		return config::maximum_lambda;
	}

	static double get_step(){
	  return config::lambda_step;
	}
};
