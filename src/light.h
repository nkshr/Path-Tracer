//Except for scene.h, don't include this.
#pragma once

class PointLight : public Object {
private:
	Spectrum m_srad;
public:
	PointLight(Vec p, Spectrum srad);
	virtual Intersection get_intersection(const Ray &r);
	virtual Spectrum illuminate(const Scene &scene, const Vec &p, const Vec &n,
		const int num_samples, unsigned short *Xi);
};

class  SpotLight : public Object {
private:
	Spectrum m_srad;
	double m_rad;
	Vec m_d;

public:
	SpotLight(Vec p, Vec t, Spectrum srad, double deg);
	virtual Intersection get_intersection(const Ray &r);
	virtual Spectrum illuminate(const Scene &scene, const Vec &p, const Vec &n,
		const int num_samples, unsigned short *Xi);
};

class Laser : public Object {
private:
	double m_r;
	double m_w;
	double m_h;
	Spectrum m_srad;
	Vec m_d;

public:
	Laser(Vec p, Vec t, double r, double h, Spectrum srad, Material cover);
	virtual Intersection get_intersection(const Ray &r);
	virtual Spectrum illuminate(const Scene &scene, const Vec &p, const Vec &n, const int num_samples, unsigned short *Xi);
};