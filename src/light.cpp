#include "light.h"

Light::Light(Vec p, Object * object) : m_position(p){
}

Laser::Laser(Vec p, Vec dir, const double r) : Light(p, new Disc(p, dir, r, Material())){

}

void Laser::illuminate(const Vec &p) {
	
}

