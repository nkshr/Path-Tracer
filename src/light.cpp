#include "light.h"

Light::Light(const Vec &p) : m_position(p){
}

Laser::Laser(const Vec &p, const Vec&dir, const double r) : Light(p), m_disc(p, dir, r, Material()){

}

void Laser::illuminate(const Vec &p) {
	
}