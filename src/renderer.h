#ifndef RENDERER_H
#define RENDERER_H

#include "vector.h"
#include "scene.h"
#include "camera.h"
#include "color.h"

class Renderer {

private:
    Scene *m_scene;
    Camera *m_camera;
	Spectrum *m_psds;
	XYZColor m_XYZ_color;

public:
    Renderer(Scene *scene, Camera *camera);
    void render();
    void save_image(const char * file_path);
	void save_spectrum_images(const char * file_path);
};

#endif //RENDERER_H