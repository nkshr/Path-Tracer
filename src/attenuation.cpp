#include <fstream>
#include <iostream>
#include <vector>
#include <map>
#include <iterator>
#include <algorithm>
#include <cstdlib>
#include <cfloat>
#include <list>
#include <cstring>

#include "common.h"
#include "attenuation.h"

bool Attenuation::load(const char *fname, const Factor factor){
	std::vector<std::pair<double, double> > coefs;
  std::ifstream ifs(fname, std::ifstream::binary);
  if (!ifs.good()) {
    std::cerr << "Warning : Couldn't open " << fname << "." << std::endl;
    return false;
  }

  const int buf_sz = 1024;
  char buf[buf_sz];
  int num_elems = 0;

  while (!ifs.eof()) {
	  ifs.getline(buf, buf_sz);
	  if (buf[0] == '\0')
		  break;

	  num_elems++;
  }

  ifs.clear();
  coefs.reserve(num_elems);
  ifs.seekg(0);

  const char * delims = ", \n\t";

  for (int i = 0; i < num_elems; ++i) {
	  ifs.getline(buf, buf_sz);
	  std::cout << buf << std::endl;
	   char * val = strtok(buf, delims);
	  double lambda = atof(val);

	  val = strtok(NULL, delims);
	  double coef = atof(val);

	  coefs.push_back(std::pair<double, double>(lambda, coef));
  }

  switch (factor) {
  case ABSORPTION:
	  m_absorp_coefs.swap(coefs);
	  break;
  case SCATTERING:
	  m_scat_coefs.swap(coefs);
	  break;
  }

  return true;
}


bool Attenuation::init() {
  if(!m_absorp_coefs.size() && !m_scat_coefs.size()){
    std::cerr << "Error : Couldn't calculate attenuation coefficients." << std::endl;
	return false;
  }
  else if (m_absorp_coefs.size() && m_scat_coefs.size()) {
    
  }
  else if (m_absorp_coefs.size()) {
    m_atten_coefs.reserve(m_absorp_coefs.size());
    copy(m_absorp_coefs.begin(), m_absorp_coefs.end(), back_inserter(m_atten_coefs));
  }
  else {
    m_scat_coefs.reserve(m_absorp_coefs.size());
    copy(m_scat_coefs.begin(), m_scat_coefs.end(), back_inserter(m_atten_coefs));
  }
  
  return true;
}

void Attenuation::scale(const double s) {
	for (int i = 0; i < m_atten_coefs.size(); ++i) {
		m_atten_coefs[i].second *= s;
	}
}

double Attenuation::sample(const double lambda) {
	double atten;
	std::vector<std::pair<double, double> >::const_iterator it = m_atten_coefs.begin();
	if (it->first >= lambda) {
		return it->second;
	}

	std::vector<std::pair<double, double> > ::const_iterator prev_it;
	while(true) {
		prev_it = it;
		++it;
		if (it == m_atten_coefs.end()) {
			atten = prev_it->second;
			break;
		}

		if (it->first >= lambda) {
			atten = linearInterpolation(lambda, prev_it->first, it->first, prev_it->second, it->second);
			break;
		}
	}

	return atten;
}

double Attenuation::get_min_lambda() const{
	return m_atten_coefs[0].first;
}

double Attenuation::get_max_lambda() const{
	return m_atten_coefs[m_atten_coefs.size() - 1].first;
}

bool Attenuation::write(const char *fname, const Factor factor) {
	std::ofstream ofs;
	ofs.open(fname);
	
	if (!ofs.good()) {
		return false;
	}

	std::vector<std::pair<double, double> > * coefs;
	switch (factor) {
	case ABSORPTION:
		coefs = &m_absorp_coefs;
		break;
	case SCATTERING:
		coefs = &m_scat_coefs;
		break;
	}

	for (std::vector<std::pair<double, double> >::iterator it = coefs->begin(); it != coefs->end(); ++it) {
		ofs << it->first << "," << it->second << "\n";
	}

	ofs.flush();
	ofs.close();
	return true;
}

double Attenuation::attenuate(const double atten_coef, const double dist, const double orig_inten) {
	return orig_inten * exp(-atten_coef * dist);
}
