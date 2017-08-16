#include <vector>

#include "tracer.h"
#include "objects.h"

ObjectIntersection Tracer::intersect(const Ray &ray) {
	ObjectIntersection isct = ObjectIntersection();
	ObjectIntersection temp;
	const size_t size = m_objects.size();

	for (int i = 0; i<size; i++) {
		temp = m_objects.at((unsigned)i)->get_intersection(ray);

		if (temp.hit) {
			if (isct.u == 0 || temp.u < isct.u) isct = temp;
		}
	}
	return isct;
}

double  * Tracer::trace_ray() {
	const int height = m_observer->get_image_height();
	const int width = m_observer->get_image_width();
	double * pixel_buffer = new double[m_observer->get_num_pixels() * 3];
	int ipb = 0;
	for (int y = 0; y < height; ++y) {
		unsigned short Xi[3] = { (unsigned short)0, (unsigned short)0,(unsigned short)(y*y*y) };               // Stores seed for erand48
		fprintf(stderr, "\rRendering : %.2f%% ",      // Prints
			(double)y / height * 100);                   // progress

		for (int x = 0; x < width; ++x) {
			Spectrum spd(0.0);
			for (int s0 = 0; s0 < m_num_samples_per_pixel; ++s0) {
				for (int s1 = 0; s1 < m_num_samples_per_point; ++s1) {
					Ray ray = m_observer->get_ray(x, y, s0 > 0, s1 > 0, Xi);
					spd = spd + trace_ray(ray, 0, Xi);
				}
			}

			Vec rgb = m_observer->convert_spd_to_rgb(spd);
			pixel_buffer[ipb++] = rgb.x;
			pixel_buffer[ipb++] = rgb.y;
			pixel_buffer[ipb++] = rgb.z;
		}
	}

	return pixel_buffer;
}


Spectrum Tracer::trace_ray(Ray ray, int depth, unsigned short * Xi) {
	ObjectIntersection isct = intersect(ray);

	// If no hit, return world colour
	if (!isct.hit) {
		return 0.0;
	}

	if (isct.m.get_type() == EMIT) {
		return m_attenuation->attenuate(isct.u, isct.m.get_spectral_emissions());
	}

	const Spectrum albedos = isct.m.get_spectral_albedos();
	if (++depth>config::maximum_depth) {
		return m_attenuation->attenuate(isct.u, isct.m.get_spectral_emissions());
	}

	Vec x = ray.origin + ray.direction * isct.u;


	Spectrum radiances(0.0);
	for (int i = 0; i < config::number_of_samples_per_intersection; ++i) {
		Ray reflected = isct.m.get_reflected_ray(ray, x, isct.n, Xi);
		radiances = radiances + trace_ray(reflected, depth, Xi) * std::max(reflected.direction.dot(isct.n), 0.0);
	}

	radiances = radiances.element_wise_product(albedos);
	radiances = radiances * 2.0 / (double)config::number_of_samples_per_intersection;
	radiances = m_attenuation->attenuate(isct.u, radiances);
	return  radiances;

}

Spectrum ShadowRay::trace_ray(Ray ray, int depth, unsigned short * Xi) {
	ObjectIntersection isct = intersect(ray);

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
			indirect_diffuse = indirect_diffuse + trace_ray(reflected, depth, Xi) * std::max(reflected.direction.dot(isct.n), 0.0);
		}

		Spectrum direct_diffuse(0.0);
		Spectrum diffuse(0.0);
		for (int i = 0; i < lights.size(); ++i) {
			Ray shadow_ray = lights[i]->get_shadow_ray(x);
			ObjectIntersection isct_temp = intersect(shadow_ray);
			const double d = (lights[i]->get_position() - x).mag();
			if (d < isct_temp.u) {
				direct_diffuse = direct_diffuse + lights[i]->get_radiance(shadow_ray);
			}
		}
		diffuse = diffuse.element_wise_product(albedos);
		diffuse = diffuse * 2.0 / (double)config::number_of_samples_per_intersection;
		diffuse = m_attenuation->attenuate(isct.u, diffuse);

		return diffuse;
	}
}