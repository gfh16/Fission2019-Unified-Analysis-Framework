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
  Int_t    GetNumOfEffEntries  (TChain*, Int_t, const char*, Int_t*, Double_t*);// chain, ssdindex, layertag, ech[], echcut[]
  Int_t    GetNumOfEntriesWithMultiCut(TChain*, Int_t, Int_t, const char*, Int_t*, Double_t*); // chain, multicut, ssdindex, layertag, ech[], echcut[]
  Double_t RatioOfMuiltiplicity(TChain*, Int_t, Int_t, const char*, Int_t*, Double_t*); // chain, multicut, ssdindex, layertag, ech[], echcut[]

};

#endif
