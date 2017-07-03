#pragma once

class c_atten_coefs {
private:
	vector<pair<double, double> > absorp_coefs;
	vector<pair<double, double> > scat_coefs;
	vector<pair<double, double> > atten_coefs;

	double step;

	bool load(const char *fname, vector<pair<double, double> > &coefs);
	
public:
	c_atten_coefs(const char *absorp_fname, const char *scat_fname);
	c_atten_coefs(const c_atten_coefs & atten) = delete;

	void scale(const double s);

	double sample(const double lambda);
	double get_min_lambda() const;
	double get_max_lambda() const;
	double get_step() const;
};

double calc_atten_coef(c_atten_coefs &atten_coefs, c_smpl_spect &spect);

inline double calc_attenuated_value(const double coef, const double z, const double color) {
	return color * exp(-coef * z);
}