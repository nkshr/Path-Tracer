//Except for scene.h, don't include this.
#pragma once
struct ObjectIntersection {
	bool hit;	// If there was an intersection
	double u;	// Distance to intersection along ray
	Vec n;		// Normal of intersected face
	Material m;	// Material of intersected face
	Object * obj;
	ObjectIntersection(bool hit_ = false, double u_ = 0, Vec n_ = Vec(), Material m_ = Material());
};