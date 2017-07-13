#pragma once

#include "spectrum.h"

class c_atten_coefs {
private:
	std::vector<std::pair<double, double> > absorp_coefs;
	std::vector<std::pair<double, double> > scat_coefs;
	std::vector<std::pair<double, double> > atten_coefs;

	bool absorp_loaded;
	bool scat_loaded;
	bool atten_loaded;
	
	double step;

	bool load(const char *fname, std::vector<std::pair<double, double> > &coefs);
	
public:
	c_atten_coefs() {};
	c_atten_coefs(const char *absorp_fname, const char *scat_fname);
	//c_atten_coefs(const c_atten_coefs & atten) = delete;

	bool init(const char *absorp_fname, const char *scat_fname);

	void scale(const double s);

	double sample(const double lambda);
	double get_min_lambda() const;
	double get_max_lambda() const;
	double get_step() const;

	bool ready() const;

	bool write_atten_coefs(const char *fname);
	bool write_absorp_coefs(const char *fname);

	double calc_atten_coef(c_smpl_spect &spect);
	
	void attenuate(const double dist, c_smpl_spect &spect);
	double attenuate(const double atten_coef, const double dist, const double orig_inten);
};

//double calc_atten_coef(c_atten_coefs &atten_coefs, c_smpl_spect &spect);

inline double calc_attenuated_value(const double coef, const double z, const double color) {
	return color * std::exp(-coef * z);
}
