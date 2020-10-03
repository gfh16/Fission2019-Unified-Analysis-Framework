#ifndef CSHINESIENERGYCALI_H
#define CSHINESIENERGYCALI_H

#include "shared.h"
#include "ReadFileModule.h"

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


#include <string.h>

//______________________________________________________________________________
class CSHINESiEnergyCali
{
public:
  CSHINESiEnergyCali();
  ~CSHINESiEnergyCali();

  void   PulserAndAlphaCali(const char*, const char*);
  void   EstimatePedestals(const char*, const char*);
  void   EstimateFitPars();
  void   CheckAbnormalChannels();
  void   EstimateDeadlayerEffects();
  void   EstimateAlphaChannelEffOnFitPars();
  void   FinallyDeterminedFitPars();
};

#endif
