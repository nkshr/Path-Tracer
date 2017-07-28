#include <iostream>

#include "spectrum.h"

#define NUM_SAMPLES 10
#define MIN_LAMBDA 0
#define MAX_LAMBDA 10
int main(int argc, char ** argv) {
	Spectrum<NUM_SAMPLES, MIN_LAMBDA, MAX_LAMBDA> a, b, c;
	a.load("");
	return 0;
}