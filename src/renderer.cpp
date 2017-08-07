#include <vector>
#include <stdio.h>
#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <cfloat>

#include "common.h"
#include "renderer.h"
#include "../lib/lodepng/lodepng.h"

Renderer::Renderer(const Scene::Config &sconfig, const Observer::Config &oconfig) {
	switch (sconfig.model) {
	default:
	case Scene::VACUUM:
		m_scene = new Vacuum(sconfig);
		break;
	case Scene::AIR:
		m_scene = new Air(sconfig);
		break;
	case Scene::UNDERWATER:
		m_scene = new Underwater(sconfig);
		break;
	}

	switch (oconfig.type) {
	default:
	case Observer::MONO:
		m_observer = new MonoCamera(oconfig);
		break;
	case Observer::RGB:
		m_observer = new RGBCamera(oconfig);
		break;
	case Observer::EYE:
		m_observer = new Eye(oconfig);
		break;
	}

	m_spds = new Spectrum[m_observer->get_width()*m_observer->get_height()];
}

Renderer::Renderer() {
	delete m_scene;
	delete m_observer;
	delete m_spds;
}

void Renderer::render() {
    int width = m_observer->get_width();
    int height = m_observer->get_height();
    double samples_recp = 1./(config::number_of_samples_per_pixel * config::number_of_samples_per_point);
	double sensor_size = m_observer->get_sensor_size();

    // Main Loop
    #pragma omp parallel for schedule(dynamic, 1)       // OpenMP
    for (int y=0; y<height; y++){
        unsigned short Xi[3]={(unsigned short)0, (unsigned short)0,(unsigned short)(y*y*y)};               // Stores seed for erand48
        fprintf(stderr, "\rRendering : %.2f%% ",      // Prints
                (double)y/height*100);                   // progress

        for (int x=0; x<width; x++){
			Spectrum &spectrum = m_spds[y * width + x];
			for (int s1 = 0; s1 < config::number_of_samples_per_pixel; ++s1) {
				for (int s2 = 0; s2 < config::number_of_samples_per_point; ++s2) {
					Ray ray = m_observer->get_ray(x, y, s1 > 0, s2 > 0, Xi);
					spectrum = spectrum + m_scene->trace_ray(ray, 0, Xi);
				}
			}
			spectrum = spectrum * samples_recp * sensor_size;
        }
    }

	m_observer->create_image(m_spds);
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
			double radiance = m_spds[j][i];
			if (max_val < radiance) {
				max_val = radiance;
			}

			if (min_val > radiance) {
				min_val = radiance;
			}
		}

		for (int j = 0; j<pixel_count; ++j) {
			double radiance = m_spds[j][i];
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
