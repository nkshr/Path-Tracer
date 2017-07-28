#include <iostream>

#include "spectrum.h"

int main(int argc, char ** argv) {
	Spectrum a("../data/spike700.csv");
	std::cout << a << std::endl;
	Spectrum b = a;
	a = a + b;
	std::cout << a << std::endl;
	return 0;
}
