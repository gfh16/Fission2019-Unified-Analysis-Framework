#ifndef CSHINEPULSERCALI_H
#define CSHINEPULSERCALI_H

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
#include "TMultiGraph.h"
#include "TGaxis.h"
#include "TLatex.h"
#include "TLine.h"

#include <string.h>


//______________________________________________________________________________
class CSHINEPulserCali
{
public:
  CSHINEPulserCali();
  ~CSHINEPulserCali();

  void   L1_AutoFindPeaksAndFit(const char* FileTag);
  void   L2_AutoFindPeaksAndFit(const char* FileTag);
  void   L1_AutoFindPeaksAndFit_ReCali(const char*, const char*);
  void   L1_GainEffectOnPulseCali_ReCali(const char* FileTag);
  void   CaliCheck_SSD_L2F_CH00_01();
  void   EstimateErrorOfHightAndSwitch();
  void   CheckLinearity(const char* layertag);
};


#endif
