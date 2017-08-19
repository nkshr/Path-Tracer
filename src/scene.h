#ifndef SCENE_H
#define SCENE_H

#include <vector>

#include "ray.h"
#include "vector.h"
#include "objects.h"
#include "observer.h"
#include "light.h"

struct Scene {
	std::vector<Object*> objects;
	std::vector<Light*> lights;
	Observer * observer;

	ObjectIntersection get_intersection(const Ray &ray) {
		ObjectIntersection isct = ObjectIntersection();
		ObjectIntersection temp;
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