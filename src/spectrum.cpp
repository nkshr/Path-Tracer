#include <fstream>
#include <cstring>
#include <cassert>

#include "spectrum.h"

#define BUF_SZ 1024
#define INITIAL_RESERVED_SZ 2

Spectrum::Spectrum(const double value, const double min_lambda, const double max_lambda, const double step){
	int num_data = ((max_lambda - min_lambda) / step)+1;
	data.reserve(num_data);
	double lambda = min_lambda;
	for (int i = 0; i < num_data; ++i) {
		data.push_back(std::pair<double, double>(lambda, value));
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


	data.resize(num_elems);

	ifs.clear();
	ifs.seekg(0);

	const char * delims = ", \n\t";
	double sum_values = 0.0;
	for (int i = 0; i < num_elems; ++i) {
		ifs.getline(buf, BUF_SZ);
		char * val = strtok(buf, delims);
		data[i].first = atof(val);

		val = strtok(NULL, delims);
		data[i].second = atof(val);
		sum_values += data[i].second;
	}

	mean_value = sum_values / num_elems;
	//for (int i = 0; i < data.size(); ++i) {
	//	std::cout << data[i].first << ", " << data[i].second << std::endl;
	//}
	return true;
}

bool Spectrum::write(const char *fname) const{
  std::ofstream ofs(fname);
  if(!ofs.good()){
    return false;
  }

  for(int i = 0; i < data.size(); ++i){
    ofs << data[i].first << "," << data[i].second << std::endl;
  }

  ofs.close();
  return true;
}

Spectrum::~Spectrum() {
}

double Spectrum::sample(const double lambda) const{
	if (!data.size())
		return mean_value;

	if (lambda < data[0].first) {
		return 0.0;
	}

	for (int i = 0; i < data.size(); ++i) {
		if (data[i].first > lambda) {
			return linearInterpolation(lambda, data[i - 1].first, data[i].first,
				data[i - 1].second, data[i].second);
		}
	}

	return 0.0;
}

void Spectrum::resize(const int i) {
	data.resize(i);
}

int Spectrum::get_num_elems() const {
	return (int)data.size();
}

void Spectrum::next(){
	++it_data;
}

void Spectrum::cur(double &lambda, double &value) {
	lambda = it_data->first;
	value = it_data->second;
}

void Spectrum::begin() {
	it_data = data.begin();
}

bool Spectrum::end() {
	return it_data == data.end();
}

void Spectrum::get_elem(const int i, double &lambda, double &value) const {
	lambda = data[i].first;
	value = data[i].second;
}

void Spectrum::set_elem(const int i, const double lambda, const double value) {
	data[i] = std::pair<double, double>(lambda, value);
}

void Spectrum::add(const double lambda, const double value) {
	data.push_back(std::pair<double, double>(lambda, value));
}

void SparseSpectrum::get_elem(const int i, double &lambda, double &value)const {
}

void SparseSpectrum::add(const double lambda, const double value) {
	data.push_back(std::pair<double, double>(lambda, value));
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
	assert(this->data.size() == spectrum.data.size());
	Spectrum ret;
	std::vector<std::pair<double, double> >::const_iterator it_a = this->data.begin();
	std::vector<std::pair<double, double> >::const_iterator end_a = this->data.end();

	ret.data.resize(this->data.size());
	std::vector<std::pair<double, double> >::iterator it_c= ret.data.begin();

	std::vector<std::pair<double, double> >::const_iterator it_b = spectrum.data.begin();

	for (; it_a != end_a; ++it_a, ++it_b) {
		it_c->second = it_a->second + it_b->second;
	}

	return Spectrum();
}
