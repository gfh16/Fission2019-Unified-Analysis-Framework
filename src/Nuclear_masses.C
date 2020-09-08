#include "../include/Nuclear_masses.h"


//******************************************************************************
// 构造函数
Nuclear_masses::Nuclear_masses (const Char_t* file_conf_name) :
fElectron_mass(0.510998918),
fUMA_MeV(931.494028)
{
  std::string line;
  Int_t N,Z,A;
  Double_t mass;
  Char_t   element[2];
  std::fstream file_in;
  file_in.open(file_conf_name, std::ios::in);
  if (!file_in.is_open()) return;
  /*allocazione*/
  fAtomic_masses = (Double_t**) new Double_t*[300];
  fElement_names = (Char_t***)  new Char_t**[300];
  for(Int_t i=0; i<300; i++)
  {
    fAtomic_masses[i] = (Double_t*) new Double_t[300];
    fElement_names[i] = (Char_t**)  new Char_t* [300];
    for(Int_t j=0; j<300; j++) {
      fElement_names[i][j] = (Char_t*) new Char_t[3]();
    }
  }
  /*lettura file*/
  for(Int_t k = 0; getline(file_in,line); k++)
  {
    if(!(line.find('*')==line.npos)) continue;
    std::istringstream buffer (line);
    buffer>>N>>Z>>A>>element>>mass;
    fAtomic_masses[Z][A] = mass;
    strcpy (fElement_names[Z][A], element);
  }
  return;
}
//******************************************************************************

//******************************************************************************
// 析构函数
Nuclear_masses::~Nuclear_masses()
{
  if(fAtomic_masses)
  {
    for(Int_t i=0; i<300; i++) {
      delete [] fAtomic_masses[i];
    }
    delete [] fAtomic_masses;
  }

  if(fElement_names)
  {
    for(Int_t i=0; i<300; i++) {
      for(Int_t j=0; j<300; j++) {
        delete [] fElement_names[i][j];
      }
      delete [] fElement_names[i];
    }
    delete [] fElement_names;
  }
}
//******************************************************************************


//******************************************************************************
Double_t Nuclear_masses::GetMass_Z_A(Int_t Z, Int_t A) const
{
  return fAtomic_masses[Z][A]*fUMA_MeV - fElectron_mass*Z;
}

//******************************************************************************
Double_t Nuclear_masses::GetMass_Z_A_uma(Int_t Z, Int_t A) const
{
  return fAtomic_masses[Z][A] - fElectron_mass*Z/fUMA_MeV;
}
