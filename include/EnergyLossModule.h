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

#include "Nuclear_masses.h"
#include "shared.h"

// NUMBER OF MODELS FOR ENERGY LOSS CALCULATIONS
static const Int_t NUM_MODELS       = 6;
static const Int_t NUM_RANGE_MODELS = 9;

class EnergyLossModule
{
public:
  EnergyLossModule();
  ~EnergyLossModule();

  Int_t LoadEnergyLossFile(const char*);
  Int_t LoadRangeFile     (const char*);
  void Clear();

  Double_t GetEnergyLoss     (Int_t Z, Int_t A, Double_t Einc,  const char* material, Double_t thickness_um, Int_t model=1);
  Double_t GetResidualEnergy (Int_t Z, Int_t A, Double_t Eloss, const char* material, Double_t thickness_um, Int_t model=1);
  Double_t GetIncidentEnergy (Int_t Z, Int_t A, Double_t Eloss, const char* material, Double_t thickness_um, Int_t model=1);

  Double_t GetRangeFromEnergy(Int_t Z, Int_t A, Double_t Einc,  const char* material, Int_t model=1);
  Double_t GetEnergyFromRange(Int_t Z, Int_t A, Double_t range, const char* material, Int_t model=1);

  void DrawdEdx(Int_t Z, Int_t A, const char* material, Int_t model=1);

private:
  std::vector <Double_t> ParticleEnergy; //MeV/u
  std::vector <Double_t> LiseELoss[NUM_MODELS];   //MeV/micron
  std::vector <Double_t> ParticleEnergyRange; //MeV/u
  std::vector <Double_t> LiseRange[NUM_RANGE_MODELS]; //micron

  Double_t Emax; //MeV/u
  Double_t Emin; //MeV/u
  Double_t ERangeMax; //MeV/u
  Double_t ERangeMin; //MeV/u
  Double_t RangeMin[NUM_RANGE_MODELS]; //micron
  Double_t RangeMax[NUM_RANGE_MODELS]; //micron

  ROOT::Math::Interpolator SplineInterpolator[NUM_MODELS];
  ROOT::Math::Interpolator RangeSplineInterpolator[NUM_RANGE_MODELS];
  ROOT::Math::Interpolator EnergyFromRangeSplineInterpolator[NUM_RANGE_MODELS];

  Nuclear_masses *NucData;

  std::string LastFileLoaded;
  std::string pathLISEInput = "/home/sea/Fission2019-Unified-Analysis-Framework/LISEInput/";
};

#endif
