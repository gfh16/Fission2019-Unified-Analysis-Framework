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

  Double_t       GetMass_Z_A    (Int_t Z, Int_t A) const;
  Double_t       GetMass_Z_A_uma(Int_t Z, Int_t A) const;

private:
  Double_t     **fAtomic_masses; // fAtomic_masses[Z][A]
  Char_t      ***fElement_names; // fElement_names[Z][A]
  Double_t       fElectron_mass; // MeV
  Double_t       fUMA_MeV;       // MeV/uma

};

#endif
