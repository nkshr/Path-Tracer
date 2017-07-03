#include <fstream>
#include <iostream>
#include <vector>
#include <map>
#include <iterator>
#include <algorithm>
#include <cstdlib>

using namespace std;

#include "common.h"
#include "spectrum.h"
#include "attenuation.h"

bool c_atten_coefs::load(const char *fname, vector<pair<double, double> > &coefs){
	ifstream ifs(fname, ios::in);
	if (!ifs.good()) {
		cerr << "Error : Couldn't open " << fname << "." << endl;
		return false;
	}

	while(true) {
		float lambda;
		ifs >> lambda;
		if (ifs.eof())
			break;

		float coef;
		ifs >> coef;

		coefs.push_back(pair<double, double>(lambda, coef));
	}

	if (coefs[0].first > coefs[1].first) {
		for (int i = 0, j = coefs.size() - 1; i < coefs.size(); ++i) {
			if (i >= j)
				break;
			//swap<double>(coefs[i], coefs[j]);
		}
	}
	return true;
}

c_atten_coefs::c_atten_coefs(const char *absorp_fname, const char *scat_fname) {
	bool absorp_loaded = load(absorp_fname, absorp_coefs);
	bool scat_loaded = load(scat_fname, scat_coefs);

	if (absorp_loaded && scat_loaded) {

	}
	else if (absorp_loaded) {
		atten_coefs.reserve(absorp_coefs.size());
		copy(absorp_coefs.begin(), absorp_coefs.end(), back_inserter(atten_coefs));
	}
	else {
		scat_coefs.reserve(absorp_coefs.size());
		copy(scat_coefs.begin(), scat_coefs.end(), back_inserter(atten_coefs));
	}

	step = (atten_coefs[atten_coefs.size() - 1].first - atten_coefs[0].first) / (atten_coefs.size() - 1);
}

void c_atten_coefs::scale(const double s) {
	for (int i = 0; i < atten_coefs.size(); ++i) {
		atten_coefs[i].second *= s;
	}
}

double c_atten_coefs::sample(const double lambda) {
	double atten;
	vector<pair<double, double> >::const_iterator it = atten_coefs.begin();
	if (it->first >= lambda) {
		return it->second;
	}

	vector<pair<double, double> > ::const_iterator prev_it;
	while(true) {
		prev_it = it;
		++it;
		if (it == atten_coefs.end()) {
			atten = prev_it->second;
			break;
		}

		if (it->first >= lambda) {
			atten = interpolate_linear(lambda, prev_it->first, it->first, prev_it->second, it->second);
			break;
		}
	}

	return atten;
}

double c_atten_coefs::get_min_lambda() const{
	return atten_coefs[0].first;
}

double c_atten_coefs::get_max_lambda() const{
	return atten_coefs[atten_coefs.size() - 1].first;
}

double c_atten_coefs::get_step() const {
	return step;
}

double calc_atten_coef(c_atten_coefs &atten_coefs, c_smpl_spect &spect) {
	if (atten_coefs.get_min_lambda() > spect.get_max_lambda())
		return 0.0;

	if (spect.get_min_lambda() > atten_coefs.get_max_lambda())
		return 0.0;

	double min_lambda = atten_coefs.get_min_lambda() > spect.get_min_lambda() 
		? atten_coefs.get_min_lambda() : spect.get_min_lambda();

	double max_lambda = atten_coefs.get_max_lambda() < spect.get_max_lambda()
		? atten_coefs.get_max_lambda() : spect.get_max_lambda();
	
	double step = atten_coefs.get_step() < spect.get_step()
		? atten_coefs.get_step() : spect.get_step();
	spect.shrink(min_lambda, max_lambda, step);
	spect.normalize();

	spect.write(NULL);

	double atten = 0.0;
	for (int i = 0; i < spect.get_num_bins(); ++i) {
		pair<double, double> spect_elem = spect.get_elem(i);
		atten += spect_elem.second * atten_coefs.sample(spect_elem.first);
	}
	return atten;
}
