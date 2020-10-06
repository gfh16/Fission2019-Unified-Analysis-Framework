#ifndef CSHINESIENERGYCALI_H
#define CSHINESIENERGYCALI_H

#include "shared.h"
#include "ReadFileModule.h"
#include "EnergyLossModule.h"

#include "TCanvas.h"
#include "TH1.h"
#include "TF1.h"
#include "TSpectrum.h"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TStyle.h"
#include "TMarker.h"
#include "TGraph.h"
#include "TLegend.h"
#include "TMultiGraph.h"
#include "TLine.h"
#include "TLatex.h"
#include "TGaxis.h"

#include <string.h>

//______________________________________________________________________________
class CSHINESiEnergyCali
{
public:
  CSHINESiEnergyCali();
  ~CSHINESiEnergyCali();

  void   PulserAndAlphaCali(const char*, const char*);
  void   EstimatePedestals(const char*, const char*);
  void   EstimateFitPars(const char*, const char*);
  void   CheckAbnormalChannels(const char*);
  void   EstimateDeadlayerEffects(const char*,const char*);
  void   EstimateAlphaChannelEffOnFitPars(const char*, const char*);
  void   FinallyDeterminedFitPars(const char*, const char*);


private:

  void   EstimateErrorOfPeak1AndPeak2(const char*, const char*);  //EstimateFitPars
  void   EstimateParameterDistributions(const char*, const char*);//EstimateFitPars

  void   Check_SSD2_L2F_CH00_CH01(const char*);    //CheckAbnormalChannels
  void   Check_SSD4_L2F_CH13_CH14(const char*);    //CheckAbnormalChannels
  void   Check_SSD4_L1S_CH00_CH15_AlphaCali00_04();//CheckAbnormalChannels

  // included by EstimateDeadlayerEffects()
  void   GetSiEnergyCaliPars(const char*,const char*,Double_t&,Double_t&,Double_t&,Int_t,Int_t,Double_t,Int_t);


};

#endif
