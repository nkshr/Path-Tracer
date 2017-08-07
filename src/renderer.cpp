#include <vector>
#include <stdio.h>
#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <cfloat>

#include "common.h"
#include "renderer.h"
#include "../lib/lodepng/lodepng.h"

Renderer::Renderer(Scene *scene, Observer *observer) {
    m_scene = scene;
    m_observer = observer;
    m_psds = new Spectrum[m_observer->get_width()*m_observer->get_height()];
}

void Renderer::render() {
    int width = m_observer->get_width();
    int height = m_observer->get_height();
    double samples_recp = 1./config::number_of_samples_per_pixel;
	double sensor_size = m_observer->get_sensor_size();

    // Main Loop
    #pragma omp parallel for schedule(dynamic, 1)       // OpenMP
    for (int y=0; y<height; y++){
        unsigned short Xi[3]={(unsigned short)0, (unsigned short)0,(unsigned short)(y*y*y)};               // Stores seed for erand48
        fprintf(stderr, "\rRendering : %.2f%% ",      // Prints
                (double)y/height*100);                   // progress

        for (int x=0; x<width; x++){
			Spectrum &spectrum = m_psds[y * width + x];
			for (int s = 0; s < config::number_of_samples_per_pixel; ++s) {
				Ray ray = m_observer->get_ray(x, y, s > 0, s>0, Xi);
				spectrum = spectrum + m_scene->trace_ray(ray, 0, Xi);
			}
			spectrum = spectrum * samples_recp * sensor_size;
        }
    }

	m_observer->create_image(m_psds);
}

void Renderer::save_image(const char *file_path) {
  DMsg dmsg("save_image");
    const int width = m_observer->get_width();
    const int height = m_observer->get_height();


	const double * pixel_buffer_d;

	int pixel_count;

	double max_val;
	double min_val;

	m_observer->read_image(pixel_buffer_d, pixel_count, max_val, min_val);

	unsigned char * pixel_buffer = new unsigned char[pixel_count * 4];
	const int jmax = pixel_count * 3;
	for (int  i = 0, j = 0; j < jmax;) {
		pixel_buffer[i++] = static_cast<int>(mapValue(pixel_buffer_d[j++], min_val, max_val, 0, 255.9));
		pixel_buffer[i++] = static_cast<int>(mapValue(pixel_buffer_d[j++], min_val, max_val, 0, 255.9));
		pixel_buffer[i++] = static_cast<int>(mapValue(pixel_buffer_d[j++], min_val, max_val, 0, 255.9));
		pixel_buffer[i++] = 255;
	}

	char complete_file_path[config::buffer_size];
	snprintf(complete_file_path, config::buffer_size, "%s.png", file_path);

	//Encode the image
	unsigned error = lodepng::encode(complete_file_path, pixel_buffer, width, height);

	//if there's an error, display it
	if (error) std::cout << "encoder error " << error << ": " << lodepng_error_text(error) << std::endl;

	delete[] pixel_buffer_d;
	delete[] pixel_buffer;
}

void Renderer::save_spectrum_images(const char * fprefix) {
  DMsg dmsg("save_spectrum_images");
	const int width = m_observer->get_width();
	const int height = m_observer->get_height();
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

		char file_path[config::buffer_size];
		snprintf(file_path, config::buffer_size, "%s_%02f.png", fprefix, config::minimum_lambda + i * config::lambda_step);

		//Encode the image
		unsigned error = lodepng::encode(file_path, pixel_buffer, width, height);

		//if there's an error, display it
		if (error) std::cout << "encoder error " << error << ": " << lodepng_error_text(error) << std::endl;

	}

}
