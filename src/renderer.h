#ifndef RENDERER_H
#define RENDERER_H

#include "vector.h"
#include "scene.h"
#include "observer.h"
#include "color.h"

class Renderer {

private:
    Scene *m_scene;
    Observer *m_observer;
	Spectrum *m_spds;
	double * m_pixel_buffer;

public:
	Renderer(const Scene::Config &sconfig, const Observer::Config &oconfig);
	Renderer();
	void render();
    void save_image(const char * file_path);
	void save_spectrum_images(const char * file_path);
};

#endif //RENDERER_H