#ifndef RAY_H
#define RAY_H

#include "vector.h"
#include "spectrum.h"

struct Ray {
    Vec origin, direction, direction_inv;

	Ray(Vec o_ = Vec(0.0), Vec d_ = Vec(0.0)) : origin(o_), direction(d_){
		direction_inv = Vec(
			1. / direction.x,
			1. / direction.y,
			1. / direction.z
		);
	}
};

#endif // RAY_H