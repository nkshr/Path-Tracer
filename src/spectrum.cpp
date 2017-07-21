#include <fstream>
#include <cstring>
#include <cassert>

#include "spectrum.h"

#define BUF_SZ 1024
#define INITIAL_RESERVED_SZ 2

Spectrum::Spectrum(const double value, const double min_lambda, const double max_lambda, const double step){
	int num_data = (int)((max_lambda - min_lambda) / step)+1;
	m_data.reserve(num_data);
	double lambda = min_lambda;
	for (int i = 0; i < num_data; ++i) {
		m_data.push_back(std::pair<double, double>(lambda, value));
		lambda += step;
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

int Spectrum::get_num_elems() const {
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

double Spectrum::get_step() const {
	return m_step;
}

void Spectrum::set_elem(const int i, const double lambda, const double value) {
	m_data[i] = std::pair<double, double>(lambda, value);
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
	for (int i = 0; i < spectrum.get_num_elems(); ++i) {
		double lambda, value;
		spectrum.get_elem(i, lambda, value);
		std::cout << lambda << ", " << value << std::endl;
	}

	return out;
}

Spectrum Spectrum::operator+(const Spectrum &spectrum) const {
	assert(this->m_data.size() == spectrum.m_data.size());

	std::vector<std::pair<double, double> >::const_iterator it_a = this->m_data.begin();
	std::vector<std::pair<double, double> >::const_iterator end_a = this->m_data.end();

	Spectrum ret;
	ret.m_data.resize(this->m_data.size());
	std::vector<std::pair<double, double> >::iterator it_c= ret.m_data.begin();

	std::vector<std::pair<double, double> >::const_iterator it_b = spectrum.m_data.begin();

	for (; it_a != end_a; ++it_a, ++it_b) {
		it_c->second = it_a->second + it_b->second;
	}

	return ret;
}

Spectrum Spectrum::operator*(const double a) const {
	std::vector<std::pair<double, double> >::const_iterator it_a = this->m_data.begin();
	std::vector<std::pair<double, double> >::const_iterator end_a = this->m_data.end();

	Spectrum ret;

	ret.m_data.resize(this->m_data.size());
	std::vector<std::pair<double, double> >::iterator it_b = ret.m_data.begin();
	for (; it_a != end_a; ++it_a) {
		it_b->second = it_a->second * a;
	}

	return ret;
}

Spectrum Spectrum::operator/(const double a) const {
	std::vector<std::pair<double, double> >::const_iterator it_a = this->m_data.begin();
	std::vector<std::pair<double, double> >::const_iterator end_a = this->m_data.end();

	Spectrum ret;

	ret.m_data.resize(this->m_data.size());
	std::vector<std::pair<double, double> >::iterator it_b = ret.m_data.begin();
	const double a_recip = 1.0 / a;
	for (; it_a != end_a; ++it_a) {
		it_b->second = it_a->second * a_recip;
	}

	return ret;
}

Spectrum Spectrum::element_wise_product(const Spectrum &spectrum) const {
	assert(this->m_data.size() == spectrum.m_data.size());

	std::vector<std::pair<double, double> >::const_iterator it_a = this->m_data.begin();
	std::vector<std::pair<double, double> >::const_iterator end_a = this->m_data.end();

	Spectrum ret;
	ret.m_data.resize(this->m_data.size());
	std::vector<std::pair<double, double> >::iterator it_c = ret.m_data.begin();

	std::vector<std::pair<double, double> >::const_iterator it_b = spectrum.m_data.begin();

	for (; it_a != end_a; ++it_a, ++it_b) {
		it_c->second = it_a->second * it_b->second;
	}

	return ret;
}