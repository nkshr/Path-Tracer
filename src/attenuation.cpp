#include "attenuation.h"

Spectrum Vacuum::attenuate(const double dist, const Spectrum &spd) {
	return spd;
}

Spectrum Water::attenuate(const double dist, const Spectrum &spd) {
	Spectrum atten_spd;
	for (int i = 0; i < config::number_of_samples_per_spectrum; ++i) {
		atten_spd[i] = spd[i] * exp(-m_atten_coefs[i] * dist);
	}
	return  atten_spd;
}

void Water::set_absorp_coefs(const Spectrum &coefs) {
	m_absorp_coefs = coefs;
	m_atten_coefs = m_absorp_coefs + m_scat_coefs;
}

void Water::set_scat_coefs(const Spectrum &coefs) {
	m_scat_coefs = coefs;
	m_atten_coefs = m_absorp_coefs + m_scat_coefs;
}

