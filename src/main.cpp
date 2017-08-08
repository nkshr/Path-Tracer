/*
*  Simple monte carlo path tracer in C++
*
*  Compilation:
*      $ mkdir build
*      $ cd build
*      $ cmake ..
*      $ make
*
*  Usage:
*      $ ./pathtracer
*/

#include <stdio.h>
#include <stdlib.h>
#include "time.h"

#include "vector.h"
#include "material.h"
#include "objects.h"
#include "observer.h"
#include "scene.h"
#include "common.h"

int main(int argc, char *argv[]) {

    time_t start, stop;
    time(&start);               // Start execution timer
	MonoCamera camera;
	camera.set_image_width(320);
	camera.set_image_height(240);
	camera.set_exposure_time(1.0);
	camera.set_position(Vec(-2.0, 0.0, 0.0));
	camera.set_target(Vec(0.0, 0.0, -6.0));
	camera.set_up(Vec(0.0, 1.0, 0.0));
	camera.set_mono_eq(Spectrum(config::gt1290_eq_file));
	camera.update();

	Scene scene;
	scene.add(new Sphere(Vec(3, 0, 6), 1, Material(EMIT, Spectrum("../data/spike700.csv"), Spectrum(0.0))));
	scene.add(new Cylinder(Vec(0, 0, 0), Vec(0, 0, 1), 4, 12, Material(DIFF)));
	
	camera.capture(scene);
	
	//write_png(observer->capture(*scene), oconfig.image_width, oconfig.image_height, "render.png");


    // Print duration information
    time(&stop);
    double diff = difftime(stop, start);
    int hrs = (int)diff/3600;
    int mins = ((int)diff/60)-(hrs*60);
    int secs = (int)diff-(hrs*3600)-(mins*60);
    printf("\rRendering : Complete!\nTime Taken: %i hrs, %i mins, %i secs\n\n", hrs, mins, secs);
    return 0;
}
