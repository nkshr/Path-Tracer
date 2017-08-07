#ifndef CONFIG_H
#define CONFIG_H
namespace config {
	static const int number_of_samples_per_spectrum = 41;
	static constexpr double minimum_lambda = 380.0;
	static constexpr double maximum_lambda = 780.0;
	static const int number_of_samples_per_pixel = 1000;
	static const int number_of_samples_per_intersection = 1;
	static const int maximum_depth = 4;

	static const char * absorption_coefficients_file = "../data/Pope_abosorp_coefs.csv";
	static const char * scattering_coefficients_file = "../data";
	static const double attenuation_coefficeints_scale = 10.0;
	static const char * gt1290_eq_file = "../data/gt1290.csv";
	static const char * mako_r_eq_file = "../data/mako_r.csv";
	static const char * mako_g_eq_file = "../data/mako_g.csv";
	static const char * mako_b_eq_file = "../data/mako_b.csv";
	static const int buffer_size = 1024;
	constexpr double lambda_step = (double)(maximum_lambda- minimum_lambda)/(double)(number_of_samples_per_spectrum - 1);
	static const double eps = 1e-4;
	static const double general_albedo = 0.18;
};
#endif
