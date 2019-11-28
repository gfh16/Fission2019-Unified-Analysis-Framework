#ifndef ENERGYLOSSMODULE_H
#define ENERGYLOSSMODULE_H

#include <fstream>
#include <sstream>
#include <vector>
#include <TGraph.h>
#include <TH1.h>
#include <TH2.h>
#include <string>
#include <math.h>
#include <cmath>
#include <Math/Interpolator.h>

#include "nuclear_masses.h"

// NUMBER OF MODELS FOR ENERGY LOSS CALCULATIONS
static const int NUM_MODELS=6;
static const int NUM_RANGE_MODELS=9;

class EnergyLossModule
{
public:
  EnergyLossModule();
  ~EnergyLossModule();
  
  int LoadEnergyLossFile(const char *);
  int LoadRangeFile(const char *);
  void Clear();
  
  double GetEnergyLoss(int Z, int A, double Einc, const char * material, double thickness_um, int model=1);
  double GetResidualEnergy(int Z, int A, double Eloss, const char * material, double thickness_um, int model=1);
  double GetIncidentEnergy(int Z, int A, double Eloss, const char * material, double thickness_um, int model=1);
  
  double GetRangeFromEnergy(int Z, int A, double Einc, const char * material, int model=1);
  double GetEnergyFromRange(int Z, int A, double range, const char * material, int model=1);
  
  void DrawdEdx(int Z, int A, const char * material, int model=1);
  
private:
  std::vector <double> ParticleEnergy; //MeV/u
  std::vector <double> LiseELoss[NUM_MODELS];   //MeV/micron
  std::vector <double> ParticleEnergyRange; //MeV/u
  std::vector <double> LiseRange[NUM_RANGE_MODELS]; //micron
  
  double Emax; //MeV/u
  double Emin; //MeV/u
  double ERangeMax; //MeV/u
  double ERangeMin; //MeV/u
  double RangeMin[NUM_RANGE_MODELS]; //micron
  double RangeMax[NUM_RANGE_MODELS]; //micron
  
  ROOT::Math::Interpolator SplineInterpolator[NUM_MODELS];
  ROOT::Math::Interpolator RangeSplineInterpolator[NUM_RANGE_MODELS];
  ROOT::Math::Interpolator EnergyFromRangeSplineInterpolator[NUM_RANGE_MODELS];
  
  nuclear_masses *NucData;
  
  std::string LastFileLoaded;
};

#endif
