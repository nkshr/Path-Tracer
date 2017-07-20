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
*      $ ./pathtracer <number of samples>
*/

#include <stdio.h>
#include <stdlib.h>
#include "time.h"

#include "vector.h"
#include "material.h"
#include "objects.h"
#include "camera.h"
#include "scene.h"
#include "renderer.h"

const char *mono_eq_fname = "../data/gt1290.csv";

int main(int argc, char *argv[]) {

    time_t start, stop;
    time(&start);               // Start execution timer
    int samples = 4;            // Default samples per pixel

    if (argc == 2) samples = atoi(argv[1]);

	Spectrum mono_eq(mono_eq_fname);
	Camera camera = Camera(Vec(0, 0, 5), Vec(0,0,0), Vec(0, 1, 0), 320, 240, mono_eq);     // Create camera
	Scene scene = Scene();                                              // Create scene

    // Add objects to scene
    //scene.add( dynamic_cast<Object*>(new Sphere(Vec(0,0,-1006), 1000, Material())) );
    //scene.add( dynamic_cast<Object*>(new Sphere(Vec(-1004,0,0), 1000, Material(DIFF, Spectrum(0.0), Spectrum("../data/spike700.csv")))));
    //scene.add( dynamic_cast<Object*>(new Sphere(Vec(1004,0,0), 1000, Material(DIFF))) );
    //scene.add( dynamic_cast<Object*>(new Sphere(Vec(0,-1000,0), 1000, Material())) );
    scene.add( dynamic_cast<Object*>(new Sphere(Vec(0,1,0), 1, Material(EMIT, Spectrum("../data/spike700.csv"), Spectrum(0.0)))) );
    scene.add( dynamic_cast<Object*>(new Cylinder(Vec(0,0,0), Vec(0,0,1), 4, 12, Material())));
    //scene.add( dynamic_cast<Object*>(new Mesh(Vec(), "../obj/dragon2.obj", Material(DIFF, Vec(0.9, 0.9, 0.9)))) );


    Renderer renderer = Renderer(&scene, &camera);  // Create renderer with our scene and camera
    renderer.render(samples);                       // Render image to pixel buffer
    renderer.save_image("render");              // Save image

    // Print duration information
    time(&stop);
    double diff = difftime(stop, start);
    int hrs = (int)diff/3600;
    int mins = ((int)diff/60)-(hrs*60);
    int secs = (int)diff-(hrs*3600)-(mins*60);
    printf("\rRendering (%i samples): Complete!\nTime Taken: %i hrs, %i mins, %i secs\n\n", samples, hrs, mins, secs);
    return 0;
}
