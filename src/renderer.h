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
	Spectrum *m_psds;

public:
    Renderer(Scene *scene, Observer *observer);
    void render();
    void save_image(const char * file_path);
	void save_spectrum_images(const char * file_path);
};

#endif //RENDERER_H