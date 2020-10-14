#ifndef CSHINEHITPATTERNRECOGNITION_H
#define CSHINEHITPATTERNRECOGNITION_H

#include "EnergyLossModule.h"
#include "CSHINESSDCalibratedData.h"
#include "TimeAndPercentage.h"
#include "shared.h"

#include <iostream>
#include <TTree.h>
#include <TChain.h>
#include <TCanvas.h>
#include <TLine.h>
#include <TGraph.h>
#include <TStyle.h>
#include <TLegend.h>
#include <TMultiGraph.h>
#include <TMath.h>


//________________________________
class CSHINEHitPatternRecognition
{
public:
  CSHINEHitPatternRecognition();
  ~CSHINEHitPatternRecognition();

  // 在这里定义需要进行的操作
  Int_t    LayerMultiplicity   (Int_t, const char*, Int_t*, Double_t*); // ssdindex, layertag, ech[], echcut[]

  Bool_t   GeoConstraint_L3A_L2B(Int_t csiindex, Int_t stripl2b);
  Bool_t   GeoConstraint_L3A_L2F(Int_t csiindex, Int_t stripl2f);
  Bool_t   GeoConstraint_L2B_L1S(Int_t stripl2b, Int_t stripl1s);
  Bool_t   GeoConstraint_L2B_L1S(Int_t stripl2b, Int_t stripl1s, Int_t deltastrip);
  Bool_t   EnergyConstraint_L2B_L2F(Double_t El2b, Double_t El2f, Double_t deltaE);

  void     EstimateLayerMulti(Int_t firstrun, Int_t lastrun);
  void     GetPunchThroughEnergiesOfLCPs();

private:
  CSHINESSDCalibratedData fCSHINESSDCalibratedData;
  EnergyLossModule        fLISEModule;
};

#endif
