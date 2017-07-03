#include <iostream>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <fstream>

using namespace std;

#include "common.h"
#include "spectrum.h"

const double c_smpl_spect::init_lambdas[] = { 361.856, 383.505, 405.155, 426.804, 448.454, 470.103, 491.753, 513.402, 535.052, 556.701, 578.351, 600.000, 621.649, 643.299, 664.948, 686.598, 708.247, 729.897, 751.546, 773.196, 794.845 };

const double c_smpl_spect::r_spect[] = { 0.09559, 0.09559, 0.08824, 0.07353, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.08824, 0.69853, 1.00000, 1.00000, 1.00000, 1.00000, 1.00000, 1.00000, 1.00000, 1.00000, 1.00000, 1.00000 };
const double c_smpl_spect::g_spect[] = { 0.00000, 0.00000, 0.00000, 0.00000, 0.03676, 0.38971, 0.78676, 1.00000, 1.00000, 0.77941, 0.31618, 0.00735, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000 };
const double c_smpl_spect::b_spect[] = { 0.99265, 1.00000, 1.00000, 1.00000, 0.86765, 0.61029, 0.30882, 0.08088, 0.00000, 0.00000, 0.00000, 0.02941, 0.05147, 0.06618, 0.06618, 0.06618, 0.06618, 0.06618, 0.06618, 0.06618, 0.06618 };

const double c_smpl_spect::c_spect[] = { 0.99265, 0.99265, 0.98529, 0.92647, 0.98529, 1.00000, 1.00000, 1.00000, 1.00000, 0.91912, 0.30147, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 0.00735, 0.00735, 0.00735, 0.00735, 0.00735 };
const double c_smpl_spect::m_spect[] = { 1.00000, 1.00000, 1.00000, 1.00000, 0.97794, 0.61765, 0.19853, 0.00000, 0.00000, 0.24265, 0.68382, 0.98529, 1.00000, 1.00000, 1.00000, 0.99265, 0.97794, 0.97794, 0.98529, 0.98529, 0.98529 };
const double c_smpl_spect::y_spect[] = { 0.00735, 0.00735, 0.00000, 0.00000, 0.30147, 0.39706, 0.69853, 0.92647, 1.00000, 1.00000, 1.00000, 0.97059, 0.95588, 0.95588, 0.95588, 0.96324, 0.97059, 0.98529, 0.99265, 1.00000, 1.00000 };

const double c_smpl_spect::w_spect[] = { 1.00000, 1.00000, 1.00000, 1.00000, 1.00000, 1.00000, 1.00000, 1.00000, 1.00000, 1.00000, 1.00000, 1.00000, 1.00000, 1.00000, 1.00000, 1.00000, 1.00000, 1.00000, 1.00000, 1.00000, 1.00000 };

c_smpl_spect::c_smpl_spect(double _r, double _g, double _b) {
	r = _r;
	g = _g;
	b = _b;

	double r_weight = 0;
	double g_weight = 0;
	double b_weight = 0;
	double c_weight = 0;
	double m_weight = 0;
	double y_weight = 0;
	double w_weight = 0;

	if( r <= g && g <= b )
	{
		w_weight = r;
		c_weight = g - r;
		b_weight = b - g;
	}
	else if( r <= b && b <= g )
	{
		w_weight = r;
		c_weight = b - r;
		g_weight = g - b;
	}
	else if( g <= r && r <= b )
	{
		w_weight = g;
		m_weight = r - g;
		b_weight = b - r;
	}
	else if( g <= b && b <= r )
	{
		w_weight = g;
		m_weight = b - g;
		r_weight = r - b;
	}
	else if( b <= r && r <= g )
	{
		w_weight = b;
		y_weight = r - b;
		g_weight = g - r;
	}
	else if( b <= g && g <= r )
	{
		w_weight = b;
		y_weight = g - b;
		r_weight = r - g;
	}

	values = new double[num_init_bins];
	for(int i=0; i< num_init_bins; ++i)
	{
		values[i] =
			w_weight * w_spect[i] +
			r_weight * r_spect[i] +
			g_weight * g_spect[i] +
			b_weight * b_spect[i] +
			c_weight * c_spect[i] +
			m_weight * m_spect[i] +
			y_weight * y_spect[i];
	}

	num_bins = num_init_bins;
	lambdas = new double[num_bins];
	memcpy((void*)lambdas, (void*)init_lambdas, sizeof(double)*num_bins);
	step = (lambdas[num_bins - 1] - lambdas[0]) / (double)(num_bins - 1);
}

c_smpl_spect::~c_smpl_spect() {
	delete values;
}

double c_smpl_spect::sample(double lambda) {
	if (lambdas[0] > lambda)
		return values[0];

	if (lambdas[num_bins - 1] < lambda) {
		return values[num_bins - 1];
	}

	double value;
	for (int i = 1; i < num_bins; ++i) {
		if (lambdas[i] > lambda) {
			value = interpolate_linear(lambda, lambdas[i - 1], lambdas[i], values[i - 1], values[i]);
			break;
		}
	}
	return value;
}


void c_smpl_spect::shrink(const double min_lambda, const double max_lambda, const double _step) {
	const int _num_bins = (int)((max_lambda - min_lambda) / _step) + 1;
	double *buf_lambdas = new double[_num_bins];
	double *buf_values = new double[_num_bins];
	
	for (int i = 0; i < _num_bins; ++i) {
		double lambda = min_lambda + _step * (double)i;
		buf_lambdas[i] = lambda;
		buf_values[i] = sample(lambda);
	}

	delete lambdas;
	delete values;
	lambdas = buf_lambdas;
	values = buf_values;
	step = _step;
	num_bins = _num_bins;
}

void c_smpl_spect::normalize() {
	double sum = 0.f;
	for (int i = 0; i < num_bins; ++i) {
		sum += values[i];
	}

	const double isum = 1.0 / sum;
	for (int i = 0; i < num_bins; ++i) {
		values[i] *= isum;
	}
}

double c_smpl_spect::get_min_lambda() const{
	return lambdas[0];
}

double c_smpl_spect::get_max_lambda() const{
	return lambdas[num_bins - 1];
}

double c_smpl_spect::get_step() const {
	return step;
}

pair<double, double> c_smpl_spect::get_elem(const int i) const {
	return pair<double, double>(lambdas[i], values[i]);
}

int c_smpl_spect::get_num_bins() const {
	return num_bins;
}

bool c_smpl_spect::write(const char *fname) {
	ofstream ofs;
	if (!fname) {
		char buf[1024];
		snprintf(buf, 1024, "smpl_spec_r%03f_g%03f_b%03f.csv", r, g, b);
		ofs.open(buf);
	}
	else {
		ofs.open(fname);
	}

	if (!ofs.good()) {
		return false;
	}

	ofs << r << ", " << g << ", " << b << "\n";
	for (int i = 0; i < num_bins; ++i) {
		ofs << lambdas[i] << ", " << values[i] << "\n";
	}

	ofs.close();
	return true;
}