#include <vector>

#include "tracer.h"
#include "scene.h"
#include  "common.h"

double  * Tracer::trace_rays() {
	const int height = m_scene.observer->get_image_height();
	const int width = m_scene.observer->get_image_width();
	double * pixel_buffer = new double[m_scene.observer->get_num_pixels() * 3];
	double sensor_size = m_scene.observer->get_sensor_height() * m_scene.observer->get_sensor_width();
		
	#pragma omp parallel for schedule(dynamic, 1)       // OpenMP
	for (int y = 0; y < height; ++y) {
		unsigned short Xi[3] = { (unsigned short)0, (unsigned short)0,(unsigned short)(y*y*y) };               // Stores seed for erand48
		fprintf(stderr, "\rRendering : %.2f%% ",      // Prints
			(double)y / height * 100);                   // progress

		for (int x = 0; x < width; ++x) {
			Spectrum spd(0.0);
			for (int s0 = 0; s0 < m_num_samples_per_pinhole; ++s0) {
				for (int s1 = 0; s1 < m_num_samples_per_pixel; ++s1) {
					Vec start = m_scene.observer->sample_point_in_pinhole(Xi);
					Vec end = m_scene.observer->sample_point_in_pixel(x, y, Xi);
					Ray ray(start, (end - start).norm());
					spd = spd + trace_ray(ray, 0, Xi);
				}

				spd = spd * sensor_size;
			}

			Vec ray_vec = (m_scene.observer->get_pixel_position(x, y) - m_scene.observer->get_position()).norm();
			Vec view_dir = (m_scene.observer->get_target() - m_scene.observer->get_position()).norm();
			double proj_pinhole_area = pow(m_scene.observer->get_pinhole_radius(), 2.f) * config::pi * std::max(view_dir.dot(ray_vec.norm()), 0.0);
			double pinhole_solid_angle = proj_pinhole_area / ray_vec.dot(ray_vec);

			spd = spd * proj_pinhole_area;


			Vec rgb = m_scene.observer->convert_spd_to_rgb(spd);
			int ipb = (y * width + x)*3;
			pixel_buffer[ipb++] = rgb.x;
			pixel_buffer[ipb++] = rgb.y;
			pixel_buffer[ipb++] = rgb.z;
		}
	}

	fprintf(stderr, "\rRendering : 100.00%%\n");
	return pixel_buffer;
}

void Tracer::set_scene(Scene scene) {
	m_scene = scene;
}

void Tracer::set_attenuation(Attenuation * attenuation) {
	m_attenuation = attenuation;
}

void Tracer::set_max_depth(int max_depth) {
	m_max_depth = max_depth;
}

void Tracer::set_num_bounces(int num_bounces) {
	m_num_bounces = num_bounces;
}

void Tracer::set_num_samples_per_pinhole(int num_samples_per_pinhole) {
	m_num_samples_per_pinhole = num_samples_per_pinhole;
}

void Tracer::set_num_samples_per_pixel(int num_samples_per_pixel) {
	m_num_samples_per_pixel = num_samples_per_pixel;
}

void Tracer::set_num_samples_per_light(int num_samples_per_light){
  m_num_samples_per_light = num_samples_per_light;
}

Spectrum PathTracer::trace_ray(const Ray &ray, int depth, unsigned short * Xi) {
	if (depth > m_max_depth)
		return Spectrum(0.0);

	Intersection isct = m_scene.get_intersection(ray);

	// If no hit, return world colour
	if (!isct.hit) {
		return Spectrum(0.0);
	}

	if (isct.m.get_type() == EMIT)  {
		if(!m_use_shadow_ray || depth == 0)
			return m_attenuation->attenuate(isct.u, isct.m.get_spectral_emissions());
		else return Spectrum(0.0);
	}
	
	const Spectrum albedos = isct.m.get_spectral_albedos();
	Vec x = ray.origin + ray.direction * isct.u;

	if (isct.m.get_type() == DIFF) {
		Spectrum indirect_diffuse(0.0);
		for (int i = 0; i < m_num_bounces; ++i) {
			Ray reflected = isct.m.get_reflected_ray(ray, x, isct.n, Xi);
			shift(isct.n, reflected);
			indirect_diffuse = indirect_diffuse + trace_ray(reflected, depth+1, Xi) * std::max(reflected.direction.dot(isct.n), 0.0);
		}

		indirect_diffuse = indirect_diffuse / (double)m_num_bounces;

		Spectrum direct_diffuse(0.0);

		if (m_use_shadow_ray) {
			for (int i = 0; i < m_scene.objects.size(); ++i) {
				if (m_scene.objects[i]->is_light()) {
					direct_diffuse = direct_diffuse + m_scene.objects[i]->illuminate(m_scene, x, isct.n, m_num_samples_per_light, Xi);
				}
			}
		}

		Spectrum diffuse = indirect_diffuse * 2 + direct_diffuse / config::pi;
		diffuse = diffuse.element_wise_product(albedos);
		diffuse = m_attenuation->attenuate(isct.u, diffuse);

		return diffuse;
	}
	
	return Spectrum(0.0);
}

void PathTracer::use_shadow_ray(bool use) {
	m_use_shadow_ray = use;
}
