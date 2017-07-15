#include <vector>
#include <stdio.h>
#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <cfloat>

using namespace std;

#include "common.h"
#include "renderer.h"
#include "../lib/lodepng/lodepng.h"



Renderer::Renderer(Scene *scene, Camera *camera) {
    m_scene = scene;
    m_camera = camera;
    m_pixel_buffer = new Vec[m_camera->get_width()*m_camera->get_height()];
}

void Renderer::render(int samples) {
    int width = m_camera->get_width();
    int height = m_camera->get_height();
    double samples_recp = 1./samples;

    // Main Loop
    #pragma omp parallel for schedule(dynamic, 1)       // OpenMP
    for (int y=0; y<height; y++){
        unsigned short Xi[3]={0,0,y*y*y};               // Stores seed for erand48

        fprintf(stderr, "\rRendering (%i samples): %.2f%% ",      // Prints
                samples, (double)y/height*100);                   // progress

        for (int x=0; x<width; x++){
            Vec col = Vec();

            for (int a=0; a<samples; a++){
                Ray ray = m_camera->get_ray(x, y, a>0, Xi);
                col = col + m_scene->trace_ray(ray, 0,Xi);
            }

            m_pixel_buffer[(y)*width + x] = col * samples_recp;
        }
    }
}

void Renderer::save_image(const char *file_path) {
    int width = m_camera->get_width();
    int height = m_camera->get_height();

    std::vector<unsigned char> pixel_buffer;

    int pixel_count = width*height;

    for (int i=0; i<pixel_count; i++) {
        pixel_buffer.push_back(toInt(m_pixel_buffer[i].x));
        pixel_buffer.push_back(toInt(m_pixel_buffer[i].y));
        pixel_buffer.push_back(toInt(m_pixel_buffer[i].z));
        pixel_buffer.push_back(255);
    }

    //Encode the image
    unsigned error = lodepng::encode(file_path, pixel_buffer, width, height);
    //if there's an error, display it
    if(error) std::cout << "encoder error " << error << ": "<< lodepng_error_text(error) << std::endl;

    pixel_buffer.clear();
}
