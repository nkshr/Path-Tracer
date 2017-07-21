#ifndef RAY_H
#define RAY_H

#include "vector.h"
#include "spectrum.h"

struct Ray {
    Vec origin, direction, direction_inv;
	Spectrum spectrum;

	//Ray(Vec o_, Vec d_) : origin(o_), direction(d_) {
 //       direction_inv = Vec(
 //               1./direction.x,
 //               1./direction.y,
 //               1./direction.z
 //       );
	//	lambda = -1;
 //   }

	Ray(Vec o_ = Vec(0.0), Vec d_ = Vec(0.0), Spectrum s_ = Spectrum()) : origin(o_), direction(d_), spectrum(s_){
		direction_inv = Vec(
			1. / direction.x,
			1. / direction.y,
			1. / direction.z
		);
	}
};

#endif // RAY_H