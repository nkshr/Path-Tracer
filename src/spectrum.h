#pragma once

class c_smpl_spect {
private:
	enum color {
		ECOLOR_RED, ECOLOR_GREEN, ECOLOR_BLUE, ECOLOR_CYAN, ECOLOR_MAGENTA, ECOLOR_YELLOW, ECOLOR_WHITE, ECOLOR_UNDEF
	};

	static const int num_init_bins= 21;
	
	static const double init_lambdas[];
	
	static const double r_spect[];
	static const double g_spect[];
	static const double b_spect[];

	static const double c_spect[];
	static const double m_spect[];
	static const double y_spect[];

	static const double w_spect[];

	double * values;
	double * lambdas;
	double step;
	int num_bins;

	double r, g, b;

public:
	c_smpl_spect(double r, double g, double b);
	
	~c_smpl_spect();

	double  sample(double lambda);
	
	void shrink(const double min_lambdas, const double max_lambdas, const double step);
	void normalize();

	double get_min_lambda() const;
	double get_max_lambda() const;
	double get_step() const;
	pair<double, double> get_elem(const int i) const;
	
	int get_num_bins() const;

	bool write(const char *fname);
};

