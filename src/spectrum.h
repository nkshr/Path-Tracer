#pragma once
#include <vector>
#include <list>
#include <utility>
#include <iostream>
#include <fstream>
#include <cstring>

#include "vector.h"
#include "common.h"

#define NUM_SAMPLES 100
#define MIN_LAMBDA 380
#define MAX_LAMBDA 780

template<int t_num_samples, int t_min_lambda, int t_max_lambda>
class SpectrumX {
private:
	double m_samples[t_num_samples];
	static double m_step;
public:
	SpectrumX(const double val = 0.0) {
		for (int i = 0; i < t_num_samples; ++i) {
			m_samples[i] = val;
		}
	}

	SpectrumX(const char *fname) {
		if (!load(fname))
			SpectrumX(0.0);
	}

	SpectrumX operator+(const SpectrumX &r) {
		SpectrumX ret;
		for (int i = 0; i < t_num_samples; ++i) {
			ret.m_samples[i] = this->m_samples[i] +r.m_samples[i];
		}
		return ret;
	}

	double operator*(const SpectrumX &r) {
		double ret = 0.0;
		for (int i = 0; i < t_num_samples; ++i) {
			ret += this->m_samples[i] * r.m_samples[i];
		}
		return ret;
	}
	
	SpectrumX operator*(const double r) {
		SpectrumX ret;
		for (int i = 0; i < t_num_samples; ++i) {
			ret.m_samples[i] *= r;
		}
		return ret;
	}
	SpectrumX operator/(const double r) {
		SpectrumX ret;
		const double r_recip = 1.0 / r;
		for (int i = 0; i < t_num_samples; ++i) {
			ret.m_samples[i] *= r_recip;
		}
		return ret;
	}
	
	double& operator[](const int i) {
		return m_samples[i];
	}
	
	std::ostream& operator<<(const std::ostream &out) {
		for (int i = 0; i < t_num_samples; ++i) {
			out << this->m_samples[i] << std::endl;
		}
	}

	bool load(const char *fname) {
		std::ifstream ifs(fname, std::ifstream::binary);
		if (!ifs.good()) {
			return false;
		}

		int num_samples = 0;

		const int buf_sz = 1024;
		char buf[buf_sz];

		while (!ifs.eof()) {
			ifs.getline(buf, buf_sz);
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
			ifs.getline(buf, buf_sz);
			char * val = strtok(buf, delims);
			samples[i].first = atof(val);

			val = strtok(NULL, delims);
			samples[i].second = atof(val);
		}

		double lambda = t_min_lambda;
		const double step = get_step();
		for (int i = 0; i < t_num_samples; ++i, lambda += step) {
			m_samples[i] = sample(samples, lambda);
		}
		return true;
	}

	SpectrumX element_wise_product(const SpectrumX &r) {
		SpectrumX ret;
		for (int i = 0; i < t_num_samples; ++i) {
			ret.m_samples[i] = this->m_samples[i] * r.m_samples[i];
		}
		return ret;
	}

	static int get_num_samples(){
		return t_num_samples;
	}

	static int get_min_lambda(){
		return t_min_lambda;
	}

	static int get_max_lambda(){
		return t_max_lambda;
	}

	static double get_step(){
		constexpr double step = (double)(t_max_lambda - t_min_lambda) / (double)(t_num_samples - 1.0);
		return step;
	}
};

typedef SpectrumX<NUM_SAMPLES, MIN_LAMBDA, MAX_LAMBDA> Spectrum;
