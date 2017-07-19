#include <vector>
#include <stdio.h>
#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <cfloat>

#include "common.h"
#include "renderer.h"
#include "../lib/lodepng/lodepng.h"

#define BUF_SZ 1024

Renderer::Renderer(Scene *scene, Camera *camera) {
    m_scene = scene;
    m_camera = camera;
    m_radiance_spectrums = new Spectrum[m_camera->get_width()*m_camera->get_height()];
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
			Ray ray;
			m_camera->get_ray_orientation(x, y, false, Xi, ray.origin, ray.direction);
            m_radiance_spectrums[y * width + x] = m_scene->trace_ray(ray, samples,Xi);
        }
    }
}

void Renderer::save_image(const char *fprefix) {
    int width = m_camera->get_width();
    int height = m_camera->get_height();

	int pixel_count = width*height;
	int lambda_count = m_radiance_spectrums[0].get_num_elems();

	for (int i = 0; i < lambda_count; ++i) {
	  std::vector<unsigned char> pixel_buffer;
	  pixel_buffer.reserve(pixel_count);
	  for (int j = 0; j<pixel_count; ++j) {
			double lambda, radiance;
			m_radiance_spectrums[j].get_elem(i, lambda, radiance);
			pixel_buffer.push_back(toInt(radiance));
			pixel_buffer.push_back(toInt(radiance));
			pixel_buffer.push_back(toInt(radiance));
			pixel_buffer.push_back(255);
		}

		double lambda, radiance;
		m_radiance_spectrums[0].get_elem(i, lambda, radiance);
		char file_path[BUF_SZ];
		snprintf(file_path, BUF_SZ, "%s_%02f.png", fprefix, lambda);
		//Encode the image
		unsigned error = lodepng::encode(file_path, pixel_buffer, width, height);
		//if there's an error, display it
		if (error) std::cout << "encoder error " << error << ": " << lodepng_error_text(error) << std::endl;

	}
}
