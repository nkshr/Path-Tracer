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
    m_psds = new Spectrum[m_camera->get_width()*m_camera->get_height()];
}

void Renderer::render() {
    int width = m_camera->get_width();
    int height = m_camera->get_height();
    double samples_recp = 1./config::number_of_samples_per_pixel;
    
    // Main Loop
    //#pragma omp parallel for schedule(dynamic, 1)       // OpenMP
    for (int y=0; y<height; y++){
        unsigned short Xi[3]={(unsigned short)0, (unsigned short)0,(unsigned short)(y*y*y)};               // Stores seed for erand48
        fprintf(stderr, "\rRendering : %.2f%% ",      // Prints
                (double)y/height*100);                   // progress

        for (int x=0; x<width; x++){
			Spectrum &spectrum = m_psds[y * width + x];
			for (int s = 0; s < config::number_of_samples_per_pixel; ++s) {
				Ray ray = m_camera->get_ray(x, y, s > 0, Xi);
				spectrum = spectrum + m_scene->trace_ray(ray, 0, Xi);
			}
			spectrum = spectrum * samples_recp;

			//spectrum = m_camera->get_mono_eq().element_wise_product(spectrum);
        }
    }
}

void Renderer::save_image(const char *file_path) {
    const int width = m_camera->get_width();
    const int height = m_camera->get_height();


	const double * pixel_buffer_d;

	int pixel_count;

	double max_val;
	double min_val;

	m_camera->read(pixel_buffer_d, pixel_count, max_val, min_val);

	unsigned char * pixel_buffer = new unsigned char[pixel_count * 4];
	const int max_buf_idx = pixel_count * 3;
	for (int  buf_idx = 0; buf_idx < max_buf_idx;) {
		pixel_buffer[buf_idx] = static_cast<int>(mapValue(pixel_buffer_d[buf_idx++], min_val, max_val, 0, 255.9));
		pixel_buffer[buf_idx] = static_cast<int>(mapValue(pixel_buffer_d[buf_idx++], min_val, max_val, 0, 255.9));
		pixel_buffer[buf_idx] = static_cast<int>(mapValue(pixel_buffer_d[buf_idx++], min_val, max_val, 0, 255.9));
		pixel_buffer[buf_idx++] = 255;
	}

	char complete_file_path[BUF_SZ];
	snprintf(complete_file_path, BUF_SZ, "%s.png", file_path);

	//Encode the image
	unsigned error = lodepng::encode(complete_file_path, pixel_buffer, width, height);

	//if there's an error, display it
	if (error) std::cout << "encoder error " << error << ": " << lodepng_error_text(error) << std::endl;

	delete[] pixel_buffer_d;
	delete[] pixel_buffer;
}

void Renderer::save_spectrum_images(const char * fprefix) {
	const int width = m_camera->get_width();
	const int height = m_camera->get_height();
	const int pixel_count = width*height;

	for (int i = 0; i < config::number_of_samples_per_spectrum; ++i) {
		std::vector<unsigned char> pixel_buffer;
		pixel_buffer.reserve(pixel_count);
		double max_val = -DBL_MAX;
		double min_val = DBL_MAX;
		for (int j = 0; j < pixel_count; ++j) {
			double radiance = m_psds[j][i];
			if (max_val < radiance) {
				max_val = radiance;
			}

			if (min_val > radiance) {
				min_val = radiance;
			}
		}

		for (int j = 0; j<pixel_count; ++j) {
			double radiance = m_psds[j][i];
			int iradiance = (int)mapValue(radiance, min_val, max_val, 0, 255.5);
			pixel_buffer.push_back(iradiance);
			pixel_buffer.push_back(iradiance);
			pixel_buffer.push_back(iradiance);
			pixel_buffer.push_back(255);
		}

		char file_path[BUF_SZ];
		snprintf(file_path, BUF_SZ, "%s_%02f.png", fprefix, config::minimum_lambda + i * config::lambda_step);

		//Encode the image
		unsigned error = lodepng::encode(file_path, pixel_buffer, width, height);

		//if there's an error, display it
		if (error) std::cout << "encoder error " << error << ": " << lodepng_error_text(error) << std::endl;

	}

}
