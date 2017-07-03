#pragma once
inline double interpolate_linear(const double  x, const double x1, const double  x2,
	const double y1, const double y2) {
	const double a = (x - x1) / (x2 - x1);
	return a * (y2 - y1) + y1;
}

inline bool in_range(const double &min, const double &max, const double &val) {
	if (min <= val && val < max) {
		return true;
	}
	return false;
}


inline void convert_hsv_to_rgb(const double h, const double s, const double v,
	double &r, double &g, double &b) {
	const double c = s * v;
	const double _h = h / 60.0;
	const double x = c * (1 - abs(fmod(_h, 2.0) - 1.0));
	if (in_range(0.0, 1.0, _h)) {
		r = c;
		g = x;
		b = 0.0;
	}
	else if (in_range(1.0, 2.0, _h)) {
		r = x;
		g = c;
		b = 0.0;
	}
	else if (in_range(2.0, 3.0, _h)) {
		r = 0.0;
		g = c;
		b = x;
	}
	else if (in_range(3.0, 4.0, _h)) {
		r = 0.0;
		g = x;
		b = c;
	}
	else if (in_range(4.0, 5.0, _h)) {
		r = x;
		g = 0.0;
		b = c;
	}
	else if (5.0 <= _h && _h <= 6.0) {
		r = c;
		g = 0.0;
		b = x;
	}
	const double m = v - c;
	r += m;
	g += m;
	b += m;
}

inline void convert_rgb_to_hsv(const double r, const double g, const double b,
	double &h, double &s, double &v) {
	const double max_val = max(r, max(g, b));
	const double min_val = min(r, min(g, b));
	double c = max_val - min_val;

	h = 0.0;
	if (abs(c) < DBL_EPSILON) {
	}
	else if (max_val == r) {
		h = fmod((g - b) / c, 6.0);
	}
	else if (max_val == g) {
		h = (b - r) / c + 2;
	}
	else {
		h = (r - g) / c + 4;
	}

	h = 60 * h;
	if (h < 0)
		h += 360;

	v = max_val;

	if (abs(v) < DBL_EPSILON)
		s = 0.0;
	else
		s = c / v;
}

// Clamp double to min/max of 0/1
inline double clamp(double x) { return x<0 ? 0 : x>1 ? 1 : x; }
// Clamp to between 0-255
inline int toInt(double x) { return int(clamp(x) * 255 + .5); }