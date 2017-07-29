#pragma once

#include <vector>
#include <utility>
#include <cmath>

#include "spectrum.h"
#include "config.h"

class Attenuation {
private:
	Spectrum atten_coefs;
	Spectrum absorp_coefs;
	Spectrum scat_coefs;

public:
	Attenuation() {
		absorp_coefs.load(config::path_of_absorption_coefficients_file);
		scat_coefs.load(config::path_of_scattering_coefficients_file);

		atten_coefs = absorp_coefs + scat_coefs;
		atten_coefs = atten_coefs  * config::scale_of_attenuation_coefficeints;
	}

	Spectrum attenuate(const double  dist, const Spectrum &orig_spect) {
		Spectrum atten_spect;
		for (int i = 0; i < config::number_of_samples_per_spectrum; ++i) {
			 atten_spect[i] = orig_spect[i] * exp(-atten_coefs[i] * dist);
		}
	}
};
