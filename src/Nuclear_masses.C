#include "../include/Nuclear_masses.h"


//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
// 构造函数
Nuclear_masses::Nuclear_masses () :
fElectron_mass(0.510998918),
fUMA_MeV(931.494028)
{
  std::string line;
  int N,Z,A;
  double mass;
  char   element[2];
  std::fstream file_in;
  file_in.open(pathMassedConf.c_str(), std::ios::in);
  if (!file_in.is_open()) return;
  /*allocazione*/
  fAtomic_masses = (double**) new double*[300];
  fElement_names = (char***)  new char**[300];
  for(int i=0; i<300; i++)
  {
    fAtomic_masses[i] = (double*) new double[300];
    fElement_names[i] = (char**)  new char* [300];
    for(int j=0; j<300; j++) {
      fElement_names[i][j] = (char*) new char[3]();
    }
  }
  /*lettura file*/
  for(int k = 0; getline(file_in,line); k++)
  {
    if(!(line.find('*')==line.npos)) continue;
    std::istringstream buffer (line);
    buffer>>N>>Z>>A>>element>>mass;
    fAtomic_masses[Z][A] = mass;
    strcpy (fElement_names[Z][A], element);
  }
  return;
}
//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo

//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
// 析构函数
Nuclear_masses::~Nuclear_masses()
{
  if(fAtomic_masses)
  {
    for(int i=0; i<300; i++) {
      delete [] fAtomic_masses[i];
    }
    delete [] fAtomic_masses;
  }

  if(fElement_names)
  {
    for(int i=0; i<300; i++) {
      for(int j=0; j<300; j++) {
        delete [] fElement_names[i][j];
      }
      delete [] fElement_names[i];
    }
    delete [] fElement_names;
  }
}
//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo


//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
double Nuclear_masses::GetMass_Z_A_MeV(int Z, int A) const
{
  return fAtomic_masses[Z][A]*fUMA_MeV - fElectron_mass*Z;
}

//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
double Nuclear_masses::GetMass_Z_A_UMA(int Z, int A) const
{
  return fAtomic_masses[Z][A] - fElectron_mass*Z/fUMA_MeV;
}
