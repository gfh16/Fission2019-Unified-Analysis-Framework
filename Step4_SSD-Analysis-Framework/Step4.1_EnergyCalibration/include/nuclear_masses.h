#ifndef NUCL_MASSES_H
#define NUCL_MASSES_H

#include <fstream>
#include <stdlib.h>
#include <string>
#include <sstream> 
#include <string.h>
#include <vector>

class nuclear_masses
{
public:
  nuclear_masses(const char * file_conf_name="masses.conf");
  ~nuclear_masses();
  
  double get_mass_Z_A(int Z, int A) const;
  double get_mass_Z_A_uma(int Z, int A) const;
  
private:
  double ** atomic_masses; // atomic_masses[Z][A]
  char *** element_names; // element_names[Z][A]
  double electron_mass; // MeV
  double UMA_MeV; // MeV/uma
  
};

#endif
