#pragma once

#include <list>
class c_smpl_spect {
private:
	enum color {
		ECOLOR_RED, ECOLOR_GREEN, ECOLOR_BLUE, ECOLOR_CYAN, ECOLOR_MAGENTA, ECOLOR_YELLOW, ECOLOR_WHITE, ECOLOR_UNDEF
	};

	static const int init_num_elems= 21;
	
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
	
	int num_elems;

	double r, g, b;

	int idx;
public:
	c_smpl_spect() {};
	c_smpl_spect(double r, double g, double b);
	
	~c_smpl_spect();

	double sample(const double lambda);

	void resize(const double min_lambda, const double max_lambda, const double step);
	
	void normalize();

	void clone(const c_smpl_spect &spect);

	double get_min_lambda() const;
	double get_max_lambda() const;
	double get_step() const;
	
	bool next();
	
	void begin();
	
	void get_elem(double &lambda, double &val) const;
	
	void set_elem(const double lambda, double val);

	bool write(const char *fname);
};

class c_spect {
public:
	struct s_elem {
		double lambda;
		double inten;
	};

private:
	std::list<s_elem> elems;
};