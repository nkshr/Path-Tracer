#include <vector>

#include "tracer.h"
#include "objects.h"
#include  "common.h"

//ObjectIntersection Tracer::intersect(const Ray &ray) {
//	ObjectIntersection isct = ObjectIntersection();
//	ObjectIntersection temp;
//	const size_t size = m_objects.size();
//
//	for (int i = 0; i<size; i++) {
//		temp = m_objects.at((unsigned)i)->get_intersection(ray);
//
//		if (temp.hit) {
//			if (isct.u == 0 || temp.u < isct.u) isct = temp;
//		}
//	}
//	return isct;
//}

double  * Tracer::trace_rays() {
	const int height = m_scene.observer->get_image_height();
	const int width = m_scene.observer->get_image_width();
	double * pixel_buffer = new double[m_scene.observer->get_num_pixels() * 3];
	double sensor_size = m_scene.observer->get_sensor_height() * m_scene.observer->get_sensor_width();
	
	int ipb = 0;
	
	//#pragma omp parallel for schedule(dynamic, 1)       // OpenMP
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

Spectrum PathTracer::trace_ray(const Ray &ray, int depth, unsigned short * Xi) {
	ObjectIntersection isct = m_scene.get_intersection(ray);

	// If no hit, return world colour
	if (!isct.hit) {
		return 0.0;
	}

	if (isct.m.get_type() == EMIT) {
		return m_attenuation->attenuate(isct.u, isct.m.get_spectral_emissions());
	}

	const Spectrum albedos = isct.m.get_spectral_albedos();
	if (++depth>m_max_depth) {
		return m_attenuation->attenuate(isct.u, isct.m.get_spectral_emissions());
	}

	Vec x = ray.origin + ray.direction * isct.u;

	Spectrum radiances(0.0);
	for (int i = 0; i < m_num_bounces; ++i) {
		Ray reflected = isct.m.get_reflected_ray(ray, x, isct.n, Xi);
		radiances = radiances + trace_ray(reflected, depth, Xi) * std::max(reflected.direction.dot(isct.n), 0.0);
	}

	radiances = radiances.element_wise_product(albedos);
	radiances = radiances * 2.0 / (double)m_num_bounces;
	radiances = m_attenuation->attenuate(isct.u, radiances);
	return  radiances;

}

Spectrum ShadowRayPathTracer::trace_ray(const Ray &ray, int depth, unsigned short * Xi) {
	ObjectIntersection isct = m_scene.get_intersection(ray);

	// If no hit, return world colour
	if (!isct.hit) {
		return 0.0;
	}

	if (isct.m.get_type() == EMIT || ++depth>m_max_depth)  {
		return m_attenuation->attenuate(isct.u, isct.m.get_spectral_emissions());
	}
	
	const Spectrum albedos = isct.m.get_spectral_albedos();
	Vec x = ray.origin + ray.direction * isct.u;

	if (isct.m.get_type() == DIFF) {
		Spectrum indirect_diffuse(0.0);
		for (int i = 0; i < m_num_bounces; ++i) {
			Ray reflected = isct.m.get_reflected_ray(ray, x, isct.n, Xi);
			shift(isct.n, reflected);
			//reflected.origin = reflected.origin + reflected.direction * config::eps;
			indirect_diffuse = indirect_diffuse + trace_ray(reflected, depth, Xi) * std::max(reflected.direction.dot(isct.n), 0.0);
		}

		Spectrum direct_diffuse(0.0);
		for (int i = 0; i < m_scene.objects.size(); ++i) {
			if (m_scene.objects[i]->is_light()) {
				direct_diffuse = direct_diffuse + m_scene.objects[i]->illuminate(m_scene, x, isct.n);
			}
		}
		//for (int i = 0; i < m_scene.lights.size(); ++i) {
		//	Ray shadow_ray(x, m_scene.lights[i]->get_shadow_ray_dir(x));

		//	shadow_ray.origin = shadow_ray.origin + shadow_ray.direction * config::eps;
		//	ObjectIntersection isct_temp = m_scene.get_intersection(shadow_ray);
		//	const double d = (m_scene.lights[i]->get_position() - x).mag();
		//	if (!isct_temp.hit || d < isct_temp.u) {
		//		direct_diffuse = direct_diffuse + m_scene.lights[i]->get_spectral_radiance(shadow_ray) * std::max(0.0, isct.n.dot(shadow_ray.direction));
		//	}
		//}

		Spectrum diffuse = indirect_diffuse + direct_diffuse;
		diffuse = diffuse.element_wise_product(albedos);
		diffuse = diffuse * 2.0 / (double)m_num_bounces;
		diffuse = m_attenuation->attenuate(isct.u, diffuse);

		return diffuse;
	}
	else {
		return Spectrum(0.0);
	}
}
