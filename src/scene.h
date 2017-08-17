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
};
#endif //SCENE_H