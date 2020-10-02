#ifndef CSHINESIENERGYCALI_H
#define CSHINESIENERGYCALI_H

#include "shared.h"

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

#include <string.h>

//______________________________________________________________________________
class CSHINESiEnergyCali
{
public:
  CSHINESiEnergyCali();
  ~CSHINESiEnergyCali();

  void   CheckAbnormalChannels();
  void   PulserAndAlphaCali();
  void   EstimatePedestals();
  void   EstimateFitPars();
  void   EstimateDeadlayerEffects();
  void   EstimateAlphaChannelEffOnFitPars();
  void   FinallyDeterminedFitPars();
};

#endif
