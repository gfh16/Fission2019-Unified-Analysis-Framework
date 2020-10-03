#ifndef NUCL_MASSES_H
#define NUCL_MASSES_H

#include <fstream>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <string.h>
#include <vector>

class Nuclear_masses
{
public:
  Nuclear_masses(const char* file_conf_name = "Masses.conf");
  ~Nuclear_masses();

  double       GetMass_Z_A    (int Z, int A) const;
  double       GetMass_Z_A_uma(int Z, int A) const;

private:
  double     **fAtomic_masses; // fAtomic_masses[Z][A]
  char      ***fElement_names; // fElement_names[Z][A]
  double       fElectron_mass; // MeV
  double       fUMA_MeV;       // MeV/uma
};

#endif
