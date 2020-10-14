#include "../include/EnergyLossModule.h"


//******************************************************************************
EnergyLossModule::EnergyLossModule()
{
  NucData = new Nuclear_masses();
}

//______________________________________________________________________________
EnergyLossModule::~EnergyLossModule()
{
  delete NucData;
}
//******************************************************************************


//******************************************************************************
void EnergyLossModule::Clear()
{
  ParticleEnergy.clear();
  for(Int_t i=0; i<NUM_MODELS; i++) {
    if(LiseELoss[i].size()) {
      LiseELoss[i].clear();
    }
  }
  for(Int_t i=0; i<NUM_RANGE_MODELS; i++) {
    if(LiseRange[i].size()) {
      LiseRange[i].clear();
    }
  }
}
//******************************************************************************


//******************************************************************************
Int_t EnergyLossModule::LoadEnergyLossFile(const char* file_name)
{
  std::ifstream FileIn(file_name);
  if(!FileIn.is_open()) {
    printf("Error: error while reading energy loss file\n");
    return -1;
  }
  if(LastFileLoaded.compare(file_name)==0) return 1;  //file already loaded

  Clear();
  Int_t NRead=0;

  while (!FileIn.eof())
  {
    std::string LineRead;
    std::getline(FileIn, LineRead);

    if(LineRead.empty()) continue;
    if(LineRead.find('*') == 0) continue;

    std::istringstream LineStream(LineRead);

    Double_t energy;
    Double_t eloss;

    for(Int_t i=0; i<NUM_MODELS; i++) {
      LineStream >> energy >> eloss;
      LiseELoss[i].push_back(eloss);
    }
    ParticleEnergy.push_back(energy);
    NRead++;
  }

  Emin=ParticleEnergy[0];
  Emax=ParticleEnergy[ParticleEnergy.size()-1];

  for(Int_t i=0; i<NUM_MODELS; i++) {
    SplineInterpolator[i].SetData(ParticleEnergy,LiseELoss[i]);
  }
  LastFileLoaded.assign(file_name);

  return NRead;
}
//******************************************************************************


//******************************************************************************
Int_t EnergyLossModule::LoadRangeFile(const char* file_name)
{
  std::ifstream FileIn(file_name);
  if(!FileIn.is_open()) {
    printf("Error: error while reading range file\n");
    return -1;
  }
  if(LastFileLoaded.compare(file_name) == 0) return 1; //file already loaded

  Clear();
  Int_t NRead=0;

  while (!FileIn.eof())
  {
    std::string LineRead;
    std::getline(FileIn, LineRead);

    if(LineRead.empty()) continue;
    if(LineRead.find('*') == 0) continue;

    std::istringstream LineStream(LineRead);

    Double_t energy;
    Double_t range;

    for(Int_t i=0; i<NUM_RANGE_MODELS; i++) {
      LineStream >> energy >> range;
      LiseRange[i].push_back(range);
    }
    ParticleEnergyRange.push_back(energy);
    NRead++;
  }

  ERangeMin = ParticleEnergyRange[0];
  ERangeMax = ParticleEnergyRange[ParticleEnergyRange.size()-1];

  for(Int_t i=0; i<NUM_RANGE_MODELS; i++) {
    RangeSplineInterpolator[i].SetData(ParticleEnergyRange,LiseRange[i]);
    EnergyFromRangeSplineInterpolator[i].SetData(LiseRange[i],ParticleEnergyRange);
    RangeMin[i] = LiseRange[i][0];
    RangeMax[i] = LiseRange[i][LiseRange[i].size()-1];
  }
  LastFileLoaded.assign(file_name);

  return NRead;
}
//******************************************************************************


//******************************************************************************
Double_t EnergyLossModule::GetEnergyLoss(Int_t Z, Int_t A, Double_t Einc,
  const char* material, Double_t thickness_um, Int_t model)
{
  Double_t Precision          =  0.0001;
  Double_t dThicknessMin      =  thickness_um*1E-4;
  Double_t IntegrateThickness =  0;
  Double_t dThickness         =  dThicknessMin;
  Double_t Eresidual          =  Einc;
  Double_t ELoss              =  0;
  Double_t mass_uma           =  NucData->GetMass_Z_A_uma(Z,A);

  if(LoadEnergyLossFile(Form("%sLISE_ELoss_Z%02d_A%02d_%s.dat", pathLISEInput.c_str(), Z, A, material))<=0) {
    printf("Error: information not present for Z=%d A=%d material=%s\n", Z, A, material);
    return -100;
  }

  for(;IntegrateThickness<thickness_um; IntegrateThickness+=dThickness) {
    if(Eresidual <= Emin*mass_uma) {  //the particle stopped in the material
      ELoss = Einc;
      return ELoss;
    }
    if(SplineInterpolator[model].Deriv(Eresidual/mass_uma)!=0) {
      dThickness = fmin(dThicknessMin,std::abs(Precision/(SplineInterpolator[model].Eval(Eresidual/mass_uma)*SplineInterpolator[model].Deriv(Eresidual/mass_uma)))); //variable integration step with fixed precision
    }
    Double_t ELossStep = dThickness*SplineInterpolator[model].Eval(Eresidual/mass_uma);

    ELoss     +=  ELossStep;
    Eresidual -=  ELossStep;
  }
  return ELoss;
}
//******************************************************************************


//******************************************************************************
Double_t EnergyLossModule::GetResidualEnergy(Int_t Z, Int_t A, Double_t Eloss,
  const char* material, Double_t thickness_um, Int_t model)
{
  Double_t Einc = GetIncidentEnergy(Z,A,Eloss,material,thickness_um, model);
  if(Einc < 0) return -1;  //the particle cannot lose this energy (energy greater than punch through energy)
  return Einc-Eloss;
}
//******************************************************************************


//******************************************************************************
Double_t EnergyLossModule::GetIncidentEnergy(Int_t Z, Int_t A, Double_t Eloss,
  const char* material, Double_t thickness_um, Int_t model)
{
  Double_t  EincStep = Eloss;
  Double_t  ElossStep;
  Double_t  dE = 30.;

  ElossStep = GetEnergyLoss(Z,A,EincStep,material,thickness_um, model);

  if(ElossStep < Eloss) return -1; //the particle cannot lose this energy (energy greater than punch through energy)

  for(;;EincStep+=dE)
  {
    ElossStep = GetEnergyLoss(Z,A,EincStep,material,thickness_um, model);

    if(ElossStep<Eloss) {
      dE = -std::abs(dE)/2;
    }
    if(ElossStep>Eloss && dE<0) {
      dE = std::abs(dE);
    }
    if(std::abs(dE)<0.00001) break;
  }
  return EincStep;
}
//******************************************************************************


//******************************************************************************
Double_t EnergyLossModule::GetRangeFromEnergy(Int_t Z, Int_t A, Double_t Einc,
  const char* material, Int_t model)
{
  Double_t mass_uma = NucData->GetMass_Z_A_uma(Z,A);

  if(LoadRangeFile(Form("%sLISE_Range_Z%02d_A%02d_%s.dat", pathLISEInput.c_str(), Z, A, material))<=0) {
    printf("Error: information not present for Z=%d A=%d material=%s\n", Z, A, material);
    return -100;
  }

  if(Einc/mass_uma>=ERangeMin && Einc/mass_uma<=ERangeMax) {
    return RangeSplineInterpolator[model].Eval(Einc/mass_uma);
  } else return -1;
}
//******************************************************************************


//******************************************************************************
Double_t EnergyLossModule::GetEnergyFromRange(Int_t Z, Int_t A, Double_t range,
  const char* material, Int_t model)
{
  Double_t mass_uma = NucData->GetMass_Z_A_uma(Z,A);

  if(LoadEnergyLossFile(Form("%sLISE_Range_Z%02d_A%02d_%s.dat", pathLISEInput.c_str(), Z, A, material))<=0) {
    printf("Error: information not present for Z=%d A=%d material=%s\n", Z, A, material);
    return -100;
  }

  if(range>=RangeMin[model] && range<=RangeMax[model]) {
    return mass_uma*EnergyFromRangeSplineInterpolator[model].Eval(range);
  } else return -1;
}
//******************************************************************************


//******************************************************************************
void EnergyLossModule::DrawdEdx(Int_t Z, Int_t A, const char* material, Int_t model)
{
  if(LoadEnergyLossFile(Form("%sLISE_ELoss_Z%02d_A%02d_%s.dat",pathLISEInput.c_str(), Z, A, material))<=0) return;

  const Int_t NPoints = ParticleEnergy.size();
  Double_t E_LISE_Values[NPoints];
  Double_t LISE_Values  [NPoints];

  const Int_t NPointsInterpolation = NPoints*1000;
  Double_t E_LISE_ValuesInterpolation[NPointsInterpolation];
  Double_t LISE_ValuesInterpolation  [NPointsInterpolation];

  for(Int_t i=0; i<NPoints; i++) {
    E_LISE_Values[i] = ParticleEnergy  [i];
    LISE_Values  [i] = LiseELoss[model][i];
  }

  for(Int_t i=0; i<NPointsInterpolation; i++) {
    E_LISE_ValuesInterpolation[i] = i*(Emax-Emin)/NPointsInterpolation;
    LISE_ValuesInterpolation  [i] = SplineInterpolator[model].Eval(E_LISE_ValuesInterpolation[i]);
  }

  TGraph* LISEGraph = new TGraph(NPoints,E_LISE_Values,LISE_Values);
  TGraph* LISEGraphInterpolation = new TGraph(NPointsInterpolation,E_LISE_ValuesInterpolation,LISE_ValuesInterpolation);

  LISEGraph->Draw("A*");
  LISEGraphInterpolation->Draw("same L");
  LISEGraphInterpolation->SetLineColor(kRed);

  return;
}
//******************************************************************************
