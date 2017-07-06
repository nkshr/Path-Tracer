#pragma once

class cie_xyz {
public:
	bool load(const char *fname);

private:
	double *xs;
	double *ys;
	double *zs;
	double *lambdas;

};