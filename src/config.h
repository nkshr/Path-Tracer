#ifndef CONFIG_H
#define CONFIG_H

namespace config {
	constexpr int number_of_samples_per_spectrum = 41;
	constexpr double minimum_lambda = 380.0;
	constexpr double maximum_lambda = 780.0;
	constexpr char* const absorption_coefficients_file = "../data/Pope_abosorp_coefs.csv";
	constexpr char* const scattering_coefficients_file = "../data";
	constexpr char* const gt1290_eq_file = "../data/gt1290.csv";
	constexpr char* const mako_r_eq_file = "../data/mako_r.csv";
	constexpr char* const mako_g_eq_file = "../data/mako_g.csv";
	constexpr char* const mako_b_eq_file = "../data/mako_b.csv";
	constexpr int buffer_size = 1024;
	constexpr double lambda_step = (double)(maximum_lambda- minimum_lambda)/(double)(number_of_samples_per_spectrum - 1);
	constexpr double eps = 1e-4;
	constexpr double general_albedo = 0.56;
	constexpr double pi = 3.14159265358979323846;
};

#endif
