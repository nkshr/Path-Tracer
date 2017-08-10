#ifndef MATERIAL_H
#define MATERIAL_H

#include "../lib/rand48/erand48.h"
#include "vector.h"
#include "ray.h"
#include "spectrum.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

enum MaterialType { DIFF, SPEC, EMIT };


class Material {

private:
	MaterialType m_type;
	Spectrum m_spectral_albedos;
	Spectrum m_spectral_emissions;

public:
	Material( MaterialType t=DIFF, Spectrum e = Spectrum(0.0), Spectrum a = Spectrum(config::general_albedo));
	MaterialType get_type() const;
	Ray get_reflected_ray( const Ray &r, Vec &p, const Vec &n, unsigned short *Xi ) const;
	Spectrum get_spectral_albedos() const;
	Spectrum get_spectral_emissions() const;
	double sample_albedo(const double lambda) const;
	double sample_emission(const double lambda) const;
};


#endif // MATERIAL_H
