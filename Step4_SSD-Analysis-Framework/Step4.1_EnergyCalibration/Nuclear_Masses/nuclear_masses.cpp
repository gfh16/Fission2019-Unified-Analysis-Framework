#include "nuclear_masses.h"

// constructor
nuclear_masses::nuclear_masses (const char * file_conf_name) : 
electron_mass(0.510998918),
UMA_MeV(931.494028)
{
  std::string line;
  int N,Z,A;
  double mass;
  char element[2];
  std::fstream file_in;
  file_in.open(file_conf_name, std::ios::in);
  if (!file_in.is_open()) return;
  /*allocazione*/
  atomic_masses=(double**) new double*[300];
  element_names=(char***) new char**[300];
  for(int i=0; i<300; i++)
  {
    atomic_masses[i]=(double*)new double[300];
    element_names[i]=(char**) new char*[300];
    for(int j=0; j<300; j++)
    {
      element_names[i][j]=(char*)new char[3](); 
    }
  }
  /*lettura file*/
  for(int k = 0; getline(file_in,line) ;k++)
  {
    if(!(line.find('*')==line.npos)) continue;
    std::istringstream buffer (line);
    buffer>>N>>Z>>A>>element>>mass;
    atomic_masses[Z][A]=mass;
    strcpy (element_names[Z][A],element);
  }
  return;
}

// destructor
nuclear_masses::~nuclear_masses()
{
  if(atomic_masses)
  {
    for(int i=0; i<300; i++)
    {
      delete [] atomic_masses[i];
    }
    delete [] atomic_masses;
  }
  if(element_names)
  {
    for(int i=0; i<300; i++)
    {
      for(int j=0; j<300; j++)
      {
        delete [] element_names[i][j];
      }
      delete [] element_names[i];
    }
    delete [] element_names;
  }
}

double nuclear_masses::get_mass_Z_A(int Z, int A) const
{
  return atomic_masses[Z][A]*UMA_MeV-electron_mass*Z;
}

double nuclear_masses::get_mass_Z_A_uma(int Z, int A) const
{
  return atomic_masses[Z][A]-electron_mass*Z/UMA_MeV;
}
