#ifndef CSHINEPEDESTALS_H
#define CSHINEPEDESTALS_H

#include "shared.h"
#include "ReadFileModule.h"

#include "TH1.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TSpectrum.h"
#include "TMath.h"

#include <iostream>


//______________________________________________________________________________
class CSHINEPedestals
{
public:
  CSHINEPedestals();
  ~CSHINEPedestals();

  void       AutoFindPedestals(const char* layertag, const char* filetag);
  void       ClickToFindPedestals(const char* layertag, const char* filetag);
  void       EstimateErrorsOfPedestals(const char* layertag, const char* filetag1, const char* filetag2);

  void       SetPoints(Int_t event, Int_t x, Int_t y, TObject* selected); //*SIGNAL*

private:
  Int_t      fIndex = 0;
  Int_t      fNPoints;
  TMarker   *m[2];
  TLine     *l[2];

};

#endif
