#include "include/EnergyLossModule.h"


//______________________________________________
EnergyLossModule::EnergyLossModule()
{
  NucData=new nuclear_masses("Nuclear_Masses/masses.conf");
}

//______________________________________________
EnergyLossModule::~EnergyLossModule()
{
  delete NucData;
}

//______________________________________________
void EnergyLossModule::Clear()
{
  ParticleEnergy.clear();
  for(int i=0; i<NUM_MODELS; i++) {
    if(LiseELoss[i].size()) {
      LiseELoss[i].clear();
    }
  }
  for(int i=0; i<NUM_RANGE_MODELS; i++) {
    if(LiseRange[i].size()) {
      LiseRange[i].clear();
    }
  }
}

//______________________________________________
int EnergyLossModule::LoadEnergyLossFile(const char * file_name)
{
  std::ifstream FileIn(file_name);
  if(!FileIn.is_open()) {
    printf("Error: error while reading energy loss file\n");
    return -1;
  }
  if(LastFileLoaded.compare(file_name)==0) return 1; //file already loaded

  Clear();
  int NRead=0;

  while (!FileIn.eof())
  {
    std::string LineRead;
    std::getline(FileIn, LineRead);

    if(LineRead.empty()) continue;
    if(LineRead.find('*')==0) continue;

    std::istringstream LineStream(LineRead);

    double energy;
    double eloss;

    for(int i=0; i<NUM_MODELS; i++) {
      LineStream >> energy >> eloss;
      LiseELoss[i].push_back(eloss);
    }

    ParticleEnergy.push_back(energy);

    NRead++;
  }

  Emin=ParticleEnergy[0];
  Emax=ParticleEnergy[ParticleEnergy.size()-1];

  for(int i=0; i<NUM_MODELS; i++) {
    SplineInterpolator[i].SetData(ParticleEnergy,LiseELoss[i]);
  }

  LastFileLoaded.assign(file_name);

  return NRead;
}

//______________________________________________
int EnergyLossModule::LoadRangeFile(const char * file_name)
{
  std::ifstream FileIn(file_name);
  if(!FileIn.is_open()) {
    printf("Error: error while reading range file\n");
    return -1;
  }
  if(LastFileLoaded.compare(file_name)==0) return 1; //file already loaded

  Clear();
  int NRead=0;

  while (!FileIn.eof())
  {
    std::string LineRead;
    std::getline(FileIn, LineRead);

    if(LineRead.empty()) continue;
    if(LineRead.find('*')==0) continue;

    std::istringstream LineStream(LineRead);

    double energy;
    double range;

    for(int i=0; i<NUM_RANGE_MODELS; i++) {
      LineStream >> energy >> range;
      LiseRange[i].push_back(range);
    }

    ParticleEnergyRange.push_back(energy);

    NRead++;
  }

  ERangeMin=ParticleEnergyRange[0];
  ERangeMax=ParticleEnergyRange[ParticleEnergyRange.size()-1];

  for(int i=0; i<NUM_RANGE_MODELS; i++) {
    RangeSplineInterpolator[i].SetData(ParticleEnergyRange,LiseRange[i]);
    EnergyFromRangeSplineInterpolator[i].SetData(LiseRange[i],ParticleEnergyRange);
    RangeMin[i]=LiseRange[i][0];
    RangeMax[i]=LiseRange[i][LiseRange[i].size()-1];
  }

  LastFileLoaded.assign(file_name);

  return NRead;
}

//______________________________________________
double EnergyLossModule::GetEnergyLoss(int Z, int A, double Einc, const char * material, double thickness_um, int model)
{
  double Precision=0.0001;
  double dThicknessMin=thickness_um*1E-4;
  double IntegrateThickness=0;
  double dThickness=dThicknessMin;
  double Eresidual=Einc;
  double ELoss=0;

  double mass_uma=NucData->get_mass_Z_A_uma(Z,A);

  if(LoadEnergyLossFile(Form("LISEInput/LISE_ELoss_Z%02d_A%02d_%s.dat", Z, A, material))<=0) {
    printf("Error: information not present for Z=%d A=%d material=%s\n", Z, A, material);
    return -100;
  }

  for(;IntegrateThickness<thickness_um; IntegrateThickness+=dThickness)
  {
    if(Eresidual<=Emin*mass_uma) { //the particle stopped in the material
      ELoss=Einc;
      return ELoss;
    }

    if(SplineInterpolator[model].Deriv(Eresidual/mass_uma)!=0) {
      dThickness=fmin(dThicknessMin,std::abs(Precision/(SplineInterpolator[model].Eval(Eresidual/mass_uma)*SplineInterpolator[model].Deriv(Eresidual/mass_uma)))); //variable integration step with fixed precision
    }

    double ELossStep=dThickness*SplineInterpolator[model].Eval(Eresidual/mass_uma);

    ELoss+=ELossStep;
    Eresidual-=ELossStep;
  }

  return ELoss;
}

//______________________________________________
double EnergyLossModule::GetResidualEnergy(int Z, int A, double Eloss, const char * material, double thickness_um, int model)
{
  double Einc=GetIncidentEnergy(Z,A,Eloss,material,thickness_um, model);
  if(Einc<0) return -1; //the particle cannot lose this energy (energy greater than punch through energy)
  return Einc-Eloss;
}

//______________________________________________
double EnergyLossModule::GetIncidentEnergy(int Z, int A, double Eloss, const char * material, double thickness_um, int model)
{
  double EincStep=Eloss;
  double ElossStep;
  double dE=30.;

  ElossStep=GetEnergyLoss(Z,A,EincStep,material,thickness_um, model);

  if(ElossStep<Eloss) return -1; //the particle cannot lose this energy (energy greater than punch through energy)

  for(;;EincStep+=dE)
  {
    ElossStep=GetEnergyLoss(Z,A,EincStep,material,thickness_um, model);

    if(ElossStep<Eloss) {
      dE=-std::abs(dE)/2;
    }
    if(ElossStep>Eloss && dE<0) {
      dE=std::abs(dE);
    }
    if(std::abs(dE)<0.00001) break;
  }

  return EincStep;
}

//______________________________________________
double EnergyLossModule::GetRangeFromEnergy(int Z, int A, double Einc, const char * material, int model)
{
  double mass_uma=NucData->get_mass_Z_A_uma(Z,A);

  if(LoadRangeFile(Form("LISEInput/LISE_Range_Z%02d_A%02d_%s.dat", Z, A, material))<=0) {
    printf("Error: information not present for Z=%d A=%d material=%s\n", Z, A, material);
    return -100;
  }

  if(Einc/mass_uma>=ERangeMin && Einc/mass_uma<=ERangeMax) {
    return RangeSplineInterpolator[model].Eval(Einc/mass_uma);
  } else return -1;
}

//______________________________________________
double EnergyLossModule::GetEnergyFromRange(int Z, int A, double range, const char * material, int model)
{
  double mass_uma=NucData->get_mass_Z_A_uma(Z,A);

  if(LoadRangeFile(Form("LISEInput/LISE_Range_Z%02d_A%02d_%s.dat", Z, A, material))<=0) {
    printf("Error: information not present for Z=%d A=%d material=%s\n", Z, A, material);
    return -100;
  }

  if(range>=RangeMin[model] && range<=RangeMax[model]) {
    return mass_uma*EnergyFromRangeSplineInterpolator[model].Eval(range);
  } else return -1;
}

//______________________________________________
void EnergyLossModule::DrawdEdx(int Z, int A, const char * material, int model)
{
  if(LoadEnergyLossFile(Form("LISEInput/LISE_ELoss_Z%02d_A%02d_%s.dat", Z, A, material))<=0) return;

  const int NPoints = ParticleEnergy.size();
  double E_LISE_Values[NPoints];
  double LISE_Values[NPoints];

  const int NPointsInterpolation = NPoints*1000;
  double E_LISE_ValuesInterpolation[NPointsInterpolation];
  double LISE_ValuesInterpolation[NPointsInterpolation];

  for(int i=0; i<NPoints; i++) {
    E_LISE_Values[i]=ParticleEnergy[i];
    LISE_Values[i]=LiseELoss[model][i];
  }

  for(int i=0; i<NPointsInterpolation; i++) {
    E_LISE_ValuesInterpolation[i]=i*(Emax-Emin)/NPointsInterpolation;
    LISE_ValuesInterpolation[i]=SplineInterpolator[model].Eval(E_LISE_ValuesInterpolation[i]);
  }

  TGraph *LISEGraph = new TGraph(NPoints,E_LISE_Values,LISE_Values);
  TGraph *LISEGraphInterpolation = new TGraph(NPointsInterpolation,E_LISE_ValuesInterpolation,LISE_ValuesInterpolation);

  LISEGraph->Draw("A*");
  LISEGraphInterpolation->Draw("same L");
  LISEGraphInterpolation->SetLineColor(kRed);

  return;
}
