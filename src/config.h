#ifndef CONFIG_H
#define CONFIG_H
namespace config {
	static const int number_of_samples_per_spectrum = 41;
	static const double minimum_lambda = 380.0;
	static const double maximum_lambda = 780.0;
	static const int number_of_samples_per_pixel = 4;
	static const int number_of_samples_per_point = 1;
	static const int maximum_depth = 4;

	static const char * path_of_absorption_coefficients_file = "../data/Pope_abosorp_coefs.csv";
	static const char * path_of_scattering_coefficients_file = "../data";
	static const double scale_of_attenuation_coefficeints = 10.0;
};
#endif