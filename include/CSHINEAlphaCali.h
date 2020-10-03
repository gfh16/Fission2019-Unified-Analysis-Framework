#ifndef CSHINEALPHACALI_H
#define CSHINEALPHACALI_H

#include "shared.h"
#include "EnergyLossModule.h"
#include "TimeAndPercentage.h"
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
#include "TLine.h"
#include "TChain.h"
#include "TGClient.h"
#include "TLegend.h"
#include "TMultiGraph.h"

#include <string.h>

//______________________________________________________________________________
class CSHINEAlphaCali
{
public:
  CSHINEAlphaCali();
  ~CSHINEAlphaCali();

  void   CalculateAlphaEnergy();
  void   MergeAlphaCaliFiles(const char*, Int_t, Int_t);
  void   ClickToFindAlphaPeaks(const char*, const char*);

  void   CheckAbnormaleChannels_SSD4();
  void   AlphaCali_SSD4_L1S_CompareAlphaCali00_04And05_08();
  void   AlphaCali_SSD4_L2F_EstimateCH13_CH14();

  void   SetPoints(Int_t event, Int_t x, Int_t y, TObject* selected); //*SIGNAL*


private:
  Int_t     fIndex = 0;
  Int_t     fNPoints;
  TMarker  *m[2];
  TLine    *l[2];
};


#endif
