#include <iostream>
#include <fstream>
#include <cstring>
#include <algorithm>
#include <list>
using namespace std;

#include "spectrum.h"
#include "common.h"
#include "color.h"

c_XYZ_color::c_XYZ_color():model(EMODEL_C3W), num_elems(0), rx(0.64), ry(0.33), 
gx(0.3), gy(0.6), bx(0.15), by(0.06),
wx(0.3333), wy(0.298), wY(106.8), step(0), min_lambda(360.0), max_lambda(830){
}

bool c_XYZ_color::init() {
	if (!load()) {
		return false;
	}

	step = (lambdas[num_elems - 1] - lambdas[0]) / static_cast<double>(num_elems-1);

	init_model();

	calc_conversion();
	return true;
}

bool c_XYZ_color::load() {
	if (num_elems > 0) {
		delete lambdas;
		delete xs;
		delete ys;
		delete zs;
	}

	ifstream ifs(fname);
	if (!ifs.good()) {
		return false;
	}

	const int buf_sz = 1024;
	char buf[buf_sz];
	num_elems = 0;

	while (!ifs.eof()) {
		ifs.getline(buf, buf_sz);
		num_elems++;
	}

	lambdas = new double[num_elems];
	xs = new double[num_elems];
	ys = new double[num_elems];
	zs = new double[num_elems];

	ifs.seekg(ios_base::beg);
	const char * delims = ", ";
	for (int i = 0; i < num_elems; ++i) {
		ifs.getline(buf, buf_sz);
		char * val = strtok(buf, delims);
		lambdas[i] = atof(val);
		val = strtok(NULL, delims);
		xs[i] = atof(val);
		val = strtok(NULL, delims);
		ys[i] = atof(val);
		val = strtok(NULL, delims);
		zs[i] = atof(val);
	}

	return true;
}

void c_XYZ_color::init_model() {
	switch (model) {
	case EMODEL_C3W:
		rx = 0.64;
		ry = 0.33;

		gx = 0.3;
		gy = 0.6;

		bx = 0.15;
		by = 0.06;

		wx = 0.3127;
		wy = 0.329;

		wY = 106.8;
		//wY = 1.0;
		break;
	default:
		break;
	}

	rz = 1.0 - rx - ry;
	gz = 1.0 - gx - gy;
	bz = 1.0 - bx - by;
	wz = 1.0 - wx - wy;
	wX = wY * wx / wy;
	wZ = wY * (1.0 - wx - wy) / wy;
}

void c_XYZ_color::set_fname(const char *fname) {
	strncpy(this->fname, fname, strlen(fname) + 1);
}

void c_XYZ_color::set_model(const e_model &model) {
	this->model = model;
}

void c_XYZ_color::set_visible_range(const double min_lambda, const double max_lambda) {
	this->min_lambda = min_lambda;
	this->max_lambda = max_lambda;
}

void c_XYZ_color::calc_XYZ(c_smpl_spect &spect, double &X, double &Y, double &Z) {
	X = Y = Z = 0;

	double lambda;
	double x, y, z;

	begin();
	do {
		get_elem(lambda, x, y, z);
		if (lambda > max_lambda)
			break;

		const double sval = spect.sample(lambda);
		
		X += x * sval * step;
		Y += y * sval * step;
		Z += z * sval * step;

	} while (next());
}

void c_XYZ_color::calc_RGB(c_smpl_spect &spect, double &R, double &G, double &B) {
	double  X, Y, Z;
	calc_XYZ(spect, X, Y, Z);
	convert_XYZ_to_RGB(X, Y, Z, R, G, B);
}

void c_XYZ_color::convert_XYZ_to_RGB(const double X, const double Y, const double Z,
	double &R, double &G, double &B) {
	R = XYZ_to_RGB[0] * X + XYZ_to_RGB[1] * Y + XYZ_to_RGB[2] * Z;
	G = XYZ_to_RGB[3] * X + XYZ_to_RGB[4] * Y + XYZ_to_RGB[5] * Z;
	B = XYZ_to_RGB[6] * X + XYZ_to_RGB[7] * Y + XYZ_to_RGB[8] * Z;
}

void c_XYZ_color::convert_RGB_to_XYZ(const double R, const double G, const double B,
	double &X, double &Y, double &Z) {
	X = RGB_to_XYZ[0] * R + RGB_to_XYZ[1] * G + RGB_to_XYZ[2] * B;
	Y = RGB_to_XYZ[3] * R + RGB_to_XYZ[4] * G + RGB_to_XYZ[5] * B;
	Z = RGB_to_XYZ[6] * R + RGB_to_XYZ[7] * G + RGB_to_XYZ[8] * B;
}

void c_XYZ_color::sample(const double lambda, double  &x, double &y, double &z) {
	if (lambda < lambdas[0]) {
		x = xs[0];
		y = ys[0];
		z = zs[0];
		return;
	}

	if (lambda > lambdas[num_elems - 1]) {
		x = xs[num_elems-1];
		y = ys[num_elems-1];
		z = zs[num_elems-1];
		return;
	}

	for (int i = 1; i < num_elems; ++i) {
		if (lambda < lambdas[i]) {
			const int prei = i - 1;
			x = interpolate_linear(lambda, lambdas[prei], lambdas[i],
				xs[prei], xs[i]);
			y = interpolate_linear(lambda, lambdas[prei], lambdas[i], ys[prei], ys[i]);
			y = interpolate_linear(lambda, lambdas[prei], lambdas[i], zs[prei], zs[i]);
			return;
		}
	}

}

void c_XYZ_color::calc_conversion() {
	//C = [rx, gx, bx]
	//    [ry, gy, by]
	//    [rz, gz, bz]

	double m[] = { rx, gx, bx,
		ry, gy, by,
		rz, gz, bz };

	double *inv_m = new double[9];
	inv_mat3x3(m, inv_m);

	const double rY_over_ry = inv_m[0] * wX + inv_m[1] * wY + inv_m[2] * wZ;
	const double gY_over_gy = inv_m[3] * wX + inv_m[4] * wY + inv_m[5] * wZ;
	const double bY_over_by = inv_m[6] * wX + inv_m[7] * wY + inv_m[8] * wZ;

	RGB_to_XYZ[0] = rx * rY_over_ry;
	RGB_to_XYZ[1] = gx * gY_over_gy;
	RGB_to_XYZ[2] = bx * bY_over_by;

	RGB_to_XYZ[3] = ry * rY_over_ry;
	RGB_to_XYZ[4] = gy * gY_over_gy;
	RGB_to_XYZ[5] = by * bY_over_by;

	RGB_to_XYZ[6] = rz * rY_over_ry;
	RGB_to_XYZ[7] = gz * gY_over_gy;
	RGB_to_XYZ[8] = bz * bY_over_by;

	inv_mat3x3(RGB_to_XYZ, XYZ_to_RGB);
}

void c_XYZ_color::begin() {
	for (int i = 0; i < num_elems; ++i) {
		if (min_lambda <= lambdas[i]) {
			idx = i;
			break;
		}
	}
}

void c_XYZ_color::get_elem(double &lambda, double &x, double &y, double &z) {
	x = xs[idx];
	y = ys[idx];
	z = zs[idx];
	lambda = lambdas[idx];
}

bool c_XYZ_color::next() {
	++idx;
	if (max_lambda >= lambdas[idx] && idx < num_elems) {
		return true;
	}
	else {
		--idx;
		return false;
	}
}

void c_XYZ_color::print_XYZ_to_RGB_conversion(ostream &os) {
	os << XYZ_to_RGB[0] << ", " << XYZ_to_RGB[1] << ", " << XYZ_to_RGB[2] << "\n";
	os << XYZ_to_RGB[3] << ", " << XYZ_to_RGB[4] << ", " << XYZ_to_RGB[5] << "\n";
	os << XYZ_to_RGB[6] << ", " << XYZ_to_RGB[7] << ", " << XYZ_to_RGB[8] << "\n";
}

void c_XYZ_color::print_RGB_to_XYZ_conversion(ostream &os) {
	os << RGB_to_XYZ[0] << ", " << RGB_to_XYZ[1] << ", " << RGB_to_XYZ[2] << "\n";
	os << RGB_to_XYZ[3] << ", " << RGB_to_XYZ[4] << ", " << RGB_to_XYZ[5] << "\n";
	os << RGB_to_XYZ[6] << ", " << RGB_to_XYZ[7] << ", " << RGB_to_XYZ[8] << "\n";
}