#include <fstream>
#include <cstring>
#include <cassert>

#include "spectrum.h"

#define BUF_SZ 1024
#define INITIAL_RESERVED_SZ 2

Spectrum::Spectrum() {
}

Spectrum::Spectrum(const double value, const double min_lambda, const double max_lambda,
	const int num_data){
	init(value, min_lambda, max_lambda, num_data);
}

void Spectrum::init(const double value,
	const double min_lambda, const double max_lambda, const int num_data) {
	m_data.reserve(num_data);
	double lambda = min_lambda;
	const double step = (max_lambda - min_lambda) / (num_data - 1);
	for (int i = 0; i < num_data; ++i, lambda += step) {
		m_data.push_back(std::pair<double, double>(lambda, value));
	}
}
Spectrum::Spectrum(const char *fname) {
	load(fname);
}

bool Spectrum::load(const char *fname) {
	std::ifstream ifs(fname, std::ifstream::binary);
	if (!ifs.good()) {
		return false;
	}

	int num_elems = 0;

	char buf[BUF_SZ];

	while (!ifs.eof()) {
		ifs.getline(buf, BUF_SZ);
		if (buf[0] == '\0')
			break;

		num_elems++;
	}


	m_data.resize(num_elems);

	ifs.clear();
	ifs.seekg(0);

	const char * delims = ", \n\t";
	double sum_values = 0.0;
	for (int i = 0; i < num_elems; ++i) {
		ifs.getline(buf, BUF_SZ);
		char * val = strtok(buf, delims);
		m_data[i].first = atof(val);

		val = strtok(NULL, delims);
		m_data[i].second = atof(val);
		sum_values += m_data[i].second;
	}

	//for (int i = 0; i < m_data.size(); ++i) {
	//	std::cout << m_data[i].first << ", " << m_data[i].second << std::endl;
	//}
	return true;
}

bool Spectrum::write(const char *fname) const{
  std::ofstream ofs(fname);
  if(!ofs.good()){
    return false;
  }

  for(int i = 0; i < m_data.size(); ++i){
    ofs << m_data[i].first << "," << m_data[i].second << std::endl;
  }

  ofs.close();
  return true;
}

Spectrum::~Spectrum() {
}

double Spectrum::sample(const double lambda) const{
	if (lambda < m_data[0].first) {
		return 0.0;
	}

	for (int i = 0; i < m_data.size(); ++i) {
		if (m_data[i].first > lambda) {
			return linearInterpolation(lambda, m_data[i - 1].first, m_data[i].first,
				m_data[i - 1].second, m_data[i].second);
		}
	}

	return 0.0;
}

void Spectrum::resize(const int i) {
	m_data.resize(i);
}

int Spectrum::get_num_data() const {
	return (int)m_data.size();
}

void Spectrum::next(){
	++m_it_data;
}

void Spectrum::cur(double &lambda, double &value) {
	lambda = m_it_data->first;
	value = m_it_data->second;
}

void Spectrum::begin() {
	m_it_data = m_data.begin();
}

bool Spectrum::end() {
	return m_it_data == m_data.end();
}

void Spectrum::get_elem(const int i, double &lambda, double &value) const {
	lambda = m_data[i].first;
	value = m_data[i].second;
}

double Spectrum::get_min_lambda() const {
	return m_data[0].first;
}

double Spectrum::get_max_lambda() const {
	return m_data[m_data.size() - 1].first;
}

void Spectrum::set_all(const double value) {
	std::vector<std::pair<double, double> >::iterator it = m_data.begin();
	std::vector<std::pair<double, double> >::const_iterator end = m_data.end();

	for (; it != end; ++it) {
		it->second = value;
	}
}

void Spectrum::add(const double lambda, const double value) {
	m_data.push_back(std::pair<double, double>(lambda, value));
}


std::ostream &operator<<(std::ostream &out, const Spectrum &spectrum) {
	for (int i = 0; i < spectrum.get_num_data(); ++i) {
		double lambda, value;
		spectrum.get_elem(i, lambda, value);
		std::cout << lambda << ", " << value << std::endl;
	}

	return out;
}

Spectrum Spectrum::element_wise_product(const Spectrum &r) const {
	const double min_lambda = this->get_min_lambda() > r.get_min_lambda() ? this->get_min_lambda() : r.get_min_lambda();
	const double max_lambda = this->get_max_lambda() > r.get_max_lambda() ? this->get_max_lambda() : r.get_max_lambda();
	const int num_data = this->get_num_data() > r.get_num_data() ? this->get_num_data() : r.get_num_data();

	

	std::vector<std::pair<double, double> >::const_iterator it_l = this->m_data.begin();
	std::vector<std::pair<double, double> >::const_iterator end = this->m_data.end();

	Spectrum s;
	s.m_data.resize(this->m_data.size());
	std::vector<std::pair<double, double> >::iterator it_s = s.m_data.begin();

	std::vector<std::pair<double, double> >::const_iterator it_r = r.m_data.begin();

	for (; it_l != end; ++it_l, ++it_r) {
		it_s->second = it_l->second * it_l->second;
	}

	return s;
}

Spectrum operator*(const Spectrum &l, const double r) {
	std::vector<std::pair<double, double> >::const_iterator it_l = l.m_data.begin();
	const std::vector<std::pair<double, double> >::const_iterator end = l.m_data.end();

	Spectrum s;
	s.m_data.resize(l.get_num_data());
	std::vector<std::pair<double, double> >::iterator it_s = s.m_data.begin();
	for (; it_l != end; ++it_l) {
		it_s->first = it_l->first;
		it_s->second = it_l->second;
	}

	return s;
}

Spectrum operator/(const Spectrum &l, const double r) {
	const double r_recip = 1.0 / r;
	return l * r_recip;
}

Spectrum operator+(const Spectrum &l, const Spectrum &r) {
	const double min_lambda = std::min(l.get_min_lambda(), r.get_min_lambda());
	const double max_lambda = std::min(l.get_max_lambda(), r.get_max_lambda());
	
	if (l.get_num_data() > r.get_num_data()) {
		Spectrum s(0, min_lambda, max_lambda, l.get_num_data());
		std::vector<std::pair<double, double> >::iterator it_s = s.m_data.begin();
		const std::vector<std::pair<double, double> >::iterator end = s.m_data.end();
		std::vector<std::pair<double, double> >::const_iterator it_l = l.m_data.begin();
		
		for (; it_s != end; ++it_l, ++it_s) {
			it_s->first = it_l->first;
			it_s->second = it_l->second + r.sample(it_l->first);
		}
		return s;
	}
	else {
		Spectrum s(0, min_lambda, max_lambda, r.get_num_data());
		std::vector<std::pair<double, double> >::iterator it_s = s.m_data.begin();
		const std::vector<std::pair<double, double> >::iterator end = s.m_data.end();
		std::vector<std::pair<double, double> >::const_iterator it_r = r.m_data.begin();

		for (; it_s != end; ++it_r, ++it_s) {
			it_s->first = it_r->first;
			it_s->second = it_r->second + l.sample(it_r->first);
		}
		return s;
	}
}

double operator*(const Spectrum &l, const Spectrum &r) {
	const double min_lambda = std::min(l.get_min_lambda(), r.get_min_lambda());
	const double max_lambda = std::min(l.get_max_lambda(), r.get_max_lambda());
	double c = 0.0;
	if (l.get_num_data() > r.get_num_data()) {
		std::vector<std::pair<double, double> >::const_iterator it_l = l.m_data.begin();
		const std::vector<std::pair<double, double> >::const_iterator end = l.m_data.end();
		for (; it_l != end; ++it_l) {
			c += it_l->second * r.sample(it_l->first);
		}
	}
	else {
		std::vector<std::pair<double, double> >::const_iterator it_b = r.m_data.begin();
		const std::vector<std::pair<double, double> >::const_iterator end =r.m_data.end();

		for (; it_b != end; ++it_b) {
			c += it_b->second * l.sample(it_b->first);
		}
	}
	return c;
}