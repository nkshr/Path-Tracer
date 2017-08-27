//Except for scene.h, don't include this.
#pragma once
struct Intersection {
	bool hit;	// If there was an intersection
	double u;	// Distance to intersection along ray
	Vec n;		// Normal of intersected face
	Material m;	// Material of intersected face
	Object * obj;
	geo::Geometry * g;
	Intersection(bool hit_ = false, double u_ = 0, Vec n_ = Vec(), Material m_ = Material(), geo::Geometry * g_ = NULL) {
		hit = hit_, u = u_, n = n_, m = m_, g = g_;
	}

};