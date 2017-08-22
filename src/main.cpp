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
#include "tracer.h"
#include "objects.h"
#include "attenuation.h"

int main(int argc, char *argv[]) {

    time_t start, stop;
    time(&start);               // Start execution timer

	Scene scene;

	//objects.push_back(new Cuboid(Vec(0.0, 0.0, 0.0), Vec(0.0, 0.0, 1.0), Vec(0.0, 1.0, 0.0), 2.0, 3.0, 1.0, Material(DIFF)));
	scene.objects.push_back(new Cylinder(Vec(0, 0, 0), Vec(0, 0, -1), 6, 12, Material(DIFF)));
	scene.objects.push_back(new Sphere(Vec(0, 0, -5), 1, DIFF));
	//scene.objects.push_back(new Sphere(Vec(3.0, 0.0, 0.0), 3, Material(EMIT, Spectrum("../data/spike700.csv"))));

	//scene.objects.push_back(new PointLight(Vec(3, 3, 0), Spectrum(1.0)));
	//scene.objects.push_back(new Laser(Vec(3, 3, 0), Vec(0, 0, -5), 1, 5, Spectrum(1.0), Material(DIFF)));
	scene.objects.push_back(new SpotLight(Vec(0, 0, 0), Vec(0, 0, -1), Spectrum(1.0), 30));
	

	MonoCamera * camera = new MonoCamera();
	camera->set_image_width(320);
	camera->set_image_height(240);
	camera->set_exposure_time(1.0);
	camera->set_sensor_width(1.0);
	camera->set_sensor_height(1.0);
	camera->set_position(Vec(0.0, 0.0, 5.0));
	camera->set_target(Vec(0.0, 0.0, -6.0));
	camera->set_up(Vec(0.0, 1.0, 0.0));
	camera->set_iso(1.0);
	camera->set_mono_eq(Spectrum(1.0));
	camera->set_medium(scene.objects[0]); //Cylinder
	camera->set_fov(60);
	camera->set_pinhole_radius(0.01);

	camera->update();

	scene.observer = camera;

	Vacuum * vacuum = new Vacuum();

	ShadowRayPathTracer * tracer = new ShadowRayPathTracer();
	tracer->set_max_depth(5);
	tracer->set_num_bounces(1);
	tracer->set_num_samples_per_pixel(20);
	tracer->set_num_samples_per_pinhole(1);
	tracer->set_attenuation(vacuum);
	tracer->set_scene(scene);
	tracer->set_type(NOT_USE);

	double * pixel_buffer = tracer->trace_rays();

	double min_val, max_val;
	getMinMax(pixel_buffer, camera->get_num_pixels()*3, min_val, max_val);
	mapValues(pixel_buffer, camera->get_num_pixels()*3, min_val, max_val, 0, 255.9);

	write_png(convertDouble3cToUchar4c(pixel_buffer, camera->get_num_pixels()), 
		camera->get_image_width(), camera->get_image_height(), "render.png");

    // Print duration information
    time(&stop);
    double diff = difftime(stop, start);
    int hrs = (int)diff/3600;
    int mins = ((int)diff/60)-(hrs*60);
    int secs = (int)diff-(hrs*3600)-(mins*60);
    printf("Time Taken: %i hrs, %i mins, %i secs\n\n", hrs, mins, secs);
    return 0;
}
