#pragma once

#include <vector>
#include <utility>
#include <cmath>

class Attenuation {
private:
	std::vector<std::pair<double, double> > m_absorp_coefs;
	std::vector<std::pair<double, double> > m_scat_coefs;
	std::vector<std::pair<double, double> > m_atten_coefs;
		
public:
	Attenuation() {};

	enum Factor {
		ABSORPTION, SCATTERING
	};

	bool init();

	void scale(const double s);

	double sample(const double lambda);
	double get_min_lambda() const;
	double get_max_lambda() const;

	bool write(const char *fname, const Factor factor);

	//double calc_atenuation(c_smpl_spect &spect);
	
	double attenuate(const double atten_coef, const double dist, const double orig_inten);

	bool load(const char *fname, const Factor factor);

};
