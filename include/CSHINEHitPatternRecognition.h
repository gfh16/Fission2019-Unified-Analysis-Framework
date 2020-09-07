#ifndef CSHINEHITPATTERNRECOGNITION_H
#define CSHINEHITPATTERNRECOGNITION_H

#include <TTree.h>
#include <TChain.h>
#include "CSHINESSDCalibratedData.h"


//________________________________
class CSHINEHitPatternRecognition
{
private:
  CSHINESSDCalibratedData fCSHINESSDCalibratedData;

public:
  CSHINEHitPatternRecognition();
  virtual ~CSHINEHitPatternRecognition();

  // 在这里定义需要进行的操作
  Int_t    LayerMultiplicity   (Int_t, const char*, Int_t*, Double_t*); // ssdindex, layertag, ech[], echcut[]

  Bool_t   GeoConstraint_L3A_L2B(Int_t csiindex, Int_t stripl2b);
  Bool_t   GeoConstraint_L3A_L2F(Int_t csiindex, Int_t stripl2f);
  Bool_t   GeoConstraint_L2B_L1S(Int_t stripl2b, Int_t stripl1s);
  Bool_t   GeoConstraint_L2B_L1S(Int_t stripl2b, Int_t stripl1s, Int_t deltastrip);

  void     EstimateLayerMulti(Int_t firstrun, Int_t lastrun);
};

#endif
