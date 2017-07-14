#include <iostream>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <fstream>
#include <cfloat>
#include <cstring>
#include <list>
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

	values = new double[init_num_elems];
	for(int i=0; i< init_num_elems; ++i)
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

	num_elems = init_num_elems;
	lambdas = new double[num_elems];
	memcpy((void*)lambdas, (void*)init_lambdas, sizeof(double)*num_elems);
	step = (lambdas[num_elems - 1] - lambdas[0]) / (double)(num_elems - 1);
}

c_smpl_spect::~c_smpl_spect() {
	delete values;
	delete lambdas;
}

double c_smpl_spect::sample(const double lambda) {
	if (lambdas[0] >= lambda)
		return values[0];

	//if (lambdas[num_elems - 1] <= lambda) {
	//	return values[num_elems - 1];
	//}

	for (int i = 1; i < num_elems; ++i) {
		if (lambdas[i] > lambda) {
			return interpolate_linear(lambda, lambdas[i - 1], lambdas[i],
				values[i - 1], values[i]);
		}
	}

	return values[num_elems - 1];
}

void c_smpl_spect::resize(const double min_lambda, const double max_lambda, const double new_step) {
	const int new_num_elems = (int)((max_lambda - min_lambda) / new_step) + 1;

	double *buf_lambdas;
	double *buf_values;
	if (init_num_elems != num_elems) {
		delete lambdas;
		delete values;

		buf_lambdas = new double[num_elems];
		buf_values = new double[num_elems];
	}
	else {
		buf_lambdas = lambdas;
		buf_values = values;
	}

	for (int i = 0; i < new_num_elems; ++i) {
		double lambda = min_lambda + new_step * (double)i;
		buf_lambdas[i] = lambda;
		buf_values[i] = sample(lambda);
	}

	lambdas = buf_lambdas;
	values = buf_values;
	num_elems = new_num_elems;
	step = new_step;
}

void c_smpl_spect::normalize() {
	double sum = 0.f;
	for (int i = 0; i < num_elems; ++i) {
		sum += values[i];
	}

	const double isum = 1.0 / sum;
	for (int i = 0; i < num_elems; ++i) {
		values[i] *= isum;
	}
}

void c_smpl_spect::clone(const c_smpl_spect &spect){
	*this = spect;
	values = new double[spect.num_elems];
	lambdas = new double[spect.num_elems];
	memcpy((void*)values, (void*)spect.values, sizeof(double) * spect.num_elems);
	memcpy((void*)lambdas, (void*)spect.lambdas, sizeof(double) * spect.num_elems);
}

double c_smpl_spect::get_min_lambda() const{
	return lambdas[0];
}

double c_smpl_spect::get_max_lambda() const{
	return lambdas[num_elems - 1];
}

double c_smpl_spect::get_step() const {
	return step;
}

bool c_smpl_spect::next(){
	++idx;
	if (idx < num_elems) {
		return true;
	}
	else {
		--idx;
		return false;
	}
}

void c_smpl_spect::begin() {
	idx = 0;
}

void c_smpl_spect::get_elem(double &lambda, double &val) const {
	lambda = lambdas[idx];
	val = values[idx];
}

void c_smpl_spect::set_elem(const double lambda, double val) {
	lambdas[idx] = lambda;
	values[idx] = val;
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
	for (int i = 0; i < num_elems; ++i) {
		ofs << lambdas[i] << ", " << values[i] << "\n";
	}

	ofs.close();
	return true;
}
