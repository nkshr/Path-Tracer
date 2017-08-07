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
#include "renderer.h"

int main(int argc, char *argv[]) {

    time_t start, stop;
    time(&start);               // Start execution timer
	Observer::Config oconfig;
	oconfig.image_width = 320;
	oconfig.image_height = 240;
	oconfig.fov = 60;
	oconfig.exposure_time = 1.0;
	oconfig.position = Vec(-2, 0, 0);
	oconfig.target = Vec(0, 0, -6);
	oconfig.up = Vec(0, 1, 0);
	oconfig.type = Observer::MONO;
	oconfig.mono_eq_file = config::gt1290_eq_file;
	oconfig.sensor_size = 1.0;


	Scene::Config sconfig;
	sconfig.model = Scene::VACUUM;
	sconfig.abosrp_coefs_file = config::absorption_coefficients_file;
	sconfig.absorp_coefs_scale = 100.0;

	sconfig.objects.push_back(new Sphere(Vec(3, 0, 6), 1, Material(EMIT, Spectrum("../data/spike700.csv"), Spectrum(0.0))));
	sconfig.objects.push_back(new Cylinder(Vec(0, 0, 0), Vec(0, 0, 1), 4, 12, Material(DIFF)));
	
	
    // Add objects to scene
    //scene.add( dynamic_cast<Object*>(new Sphere(Vec(0,0,-1006), 1000, Material())) );
    //scene.add( dynamic_cast<Object*>(new Sphere(Vec(-1004,0,0), 1000, Material(DIFF, Spectrum(0.0), Spectrum("../data/spike700.csv")))));
    //scene.add( dynamic_cast<Object*>(new Sphere(Vec(1004,0,0), 1000, Material(DIFF))) );
    //scene.add( dynamic_cast<Object*>(new Sphere(Vec(0,-1000,0), 1000, Material())) );
    //scene->add( dynamic_cast<Object*>(new Sphere(Vec(3,0,6), 1, Material(EMIT, Spectrum("../data/spike700.csv"), Spectrum(0.0)))) );
    //scene->add( dynamic_cast<Object*>(new Cylinder(Vec(0,0,0), Vec(0,0,1), 4, 12, Material(DIFF))));
    //scene.add( dynamic_cast<Object*>(new Mesh(Vec(), "../obj/dragon2.obj", Material(DIFF, Vec(0.9, 0.9, 0.9)))) );


    Renderer renderer = Renderer(sconfig, oconfig);  // Create renderer with our scene and camera
    renderer.render();                       // Render image to pixel buffer
    renderer.save_image("render");              // Save image

    // Print duration information
    time(&stop);
    double diff = difftime(stop, start);
    int hrs = (int)diff/3600;
    int mins = ((int)diff/60)-(hrs*60);
    int secs = (int)diff-(hrs*3600)-(mins*60);
    printf("\rRendering : Complete!\nTime Taken: %i hrs, %i mins, %i secs\n\n", hrs, mins, secs);
    return 0;
}
