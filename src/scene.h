#ifndef SCENE_H
#define SCENE_H

#include <vector>

#include "ray.h"
#include "vector.h"
#include "material.h"
#include "geometry.h"

struct Scene;
class Object;

#include "intersection.h"
#include "object.h"
#include "color.h"
#include "observer.h"
#include "light.h"

struct Scene {
	std::vector<Object*> objects;
	Observer * observer;

	Intersection get_intersection(const Ray &ray) const {
		Intersection isct = Intersection();
		Intersection temp;
		const size_t size = objects.size();

		for (int i = 0; i<size; i++) {
			temp = objects.at((unsigned)i)->get_intersection(ray);

			if (temp.hit) {
				if (isct.u == 0 || temp.u < isct.u) {
					isct = temp;
					if (objects[i] == observer->get_medium()) {
						isct.n = isct.n * -1;
					}
				}
			}
		}
		return isct;
	}

};
#endif //SCENE_H