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
using namespace std;

#include "common.h"
#include "attenuation.h"

bool Attenuation::load(const char *fname, vector<pair<double, double> > &coefs){
  ifstream ifs(fname, ifstream::binary);
  if (!ifs.good()) {
    cerr << "Warning : Couldn't open " << fname << "." << endl;
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
	  cout << buf << endl;
	   char * val = strtok(buf, delims);
	  double lambda = atof(val);

	  val = strtok(NULL, delims);
	  double coef = atof(val);

	  coefs.push_back(pair<double, double>(lambda, coef));
  }

  return true;
}

Attenuation::Attenuation(const char *absorp_fname, const char *scat_fname) {
	init(absorp_fname, scat_fname);
}

bool Attenuation::init(const char *absorp_fname, const char *scat_fname) {
  absorp_loaded = false;
  scat_loaded = false;
  atten_loaded = false;
  absorp_loaded = load(absorp_fname, absorp_coefs);
  scat_loaded = load(scat_fname, scat_coefs);	

  if(!absorp_loaded && !scat_loaded){
    cerr << "Error : Couldn't calculate attenuation coefficients." << endl;
  }
  else if (absorp_loaded && scat_loaded) {
    
  }
  else if (absorp_loaded) {
    atten_coefs.reserve(absorp_coefs.size());
    copy(absorp_coefs.begin(), absorp_coefs.end(), back_inserter(atten_coefs));
  }
  else {
    scat_coefs.reserve(absorp_coefs.size());
    copy(scat_coefs.begin(), scat_coefs.end(), back_inserter(atten_coefs));
  }

  if(atten_coefs.size() > 0){
	  step = (atten_coefs[atten_coefs.size() - 1].first - atten_coefs[0].first) / (atten_coefs.size() - 1);
	  atten_loaded = true;
  }else{
	  step = 0.0;
  }
  
  return atten_loaded;
}

void Attenuation::scale(const double s) {
	for (int i = 0; i < atten_coefs.size(); ++i) {
		atten_coefs[i].second *= s;
	}
}

double Attenuation::sample(const double lambda) {
	double atten;
	vector<pair<double, double> >::const_iterator it = atten_coefs.begin();
	if (it->first >= lambda) {
		return it->second;
	}

	vector<pair<double, double> > ::const_iterator prev_it;
	while(true) {
		prev_it = it;
		++it;
		if (it == atten_coefs.end()) {
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
	return atten_coefs[0].first;
}

double Attenuation::get_max_lambda() const{
	return atten_coefs[atten_coefs.size() - 1].first;
}

double Attenuation::get_step() const {
	return step;
}

bool Attenuation::ready() const{
  return atten_loaded;
}

bool Attenuation::write_atten_coefs(const char *fname) {
	ofstream ofs;
	ofs.open(fname);
	
	if (!ofs.good()) {
		return false;
	}

	for (int i = 0; i < atten_coefs.size(); ++i) {
		ofs << atten_coefs[i].first << "," << atten_coefs[i].second << "\n";
	}

	ofs.flush();
	ofs.close();
	return true;
}

bool Attenuation::write_absorp_coefs(const char *fname) {
	ofstream ofs;
	ofs.open(fname);

	if (!ofs.good()) {
		return false;
	}

	for (int i = 0; i < absorp_coefs.size(); ++i) {
		ofs << absorp_coefs[i].first << "," << absorp_coefs[i].second << "\n";
	}

	ofs.flush();
	ofs.close();
	return true;
}

double Attenuation::attenuate(const double atten_coef, const double dist, const double orig_inten) {
	return orig_inten * exp(-atten_coef * dist);
}
