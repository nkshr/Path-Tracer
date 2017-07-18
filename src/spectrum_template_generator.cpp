#include <iostream>
#include <fstream>
#include <cfloat>
using namespace std;

const char *fname = "../data/spectrum_template.csv";
const double min_lambda = 380.0;
const double max_lambda = 780.0;
const double step = 1.0;
const double value = 1.8;
int main(int argc, char ** argv) {
	ofstream ofs(fname);
	if (!ofs.good())
	{
		cerr << "Couldn't open " << fname << endl;
		return false;
	}
	for (double lambda = min_lambda; lambda - max_lambda < DBL_EPSILON; lambda += step) {
		ofs << lambda << "," << value << endl;
	}

	ofs.close();
	return 0;
}
