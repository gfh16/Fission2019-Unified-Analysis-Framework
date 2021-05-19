#ifndef CSHINETrackReconstruction_H
#define CSHINETrackReconstruction_H

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
class CSHINETrackReconstruction
{
public:
  CSHINETrackReconstruction();
  ~CSHINETrackReconstruction();

  //   在这里定义需要进行的操作
  // ------------------------
  // 计算每一层的多重数
  Int_t    LayerMultiplicity   (Int_t, const char*, Int_t*, Double_t*); // ssdindex, layertag, ech[], echcut[]

  // L3A_L2B 几何约束
  Bool_t   IsGeoConstraint_L3A_L2B(Int_t csiindex, Int_t  stripl2b);
  Bool_t   IsGeoConstraint_L3A_L2B(Int_t ssdindex, Int_t csinum, Int_t csissdnum,Int_t l2bmulti, Int_t* l2bstrip, Int_t* l2bssdnum);
  Bool_t   IsGeoConstraint_L3A_L2B(Int_t ssdindex, Int_t csimulti, Int_t* csinum, Int_t* csissdnum, Int_t l2bstrip, Int_t l2bssdnum);
  Bool_t   IsGeoConstraint_L3A_L2B(Int_t ssdindex, Int_t csimulti, Int_t* csinum,Int_t* csissdnum, Int_t l2bmulti, Int_t* l2bstrip, Int_t* l2bssdnum);
  // L3A_L2F 几何约束
  Bool_t   IsGeoConstraint_L3A_L2F(Int_t csiindex, Int_t stripl2f);
  Bool_t   IsGeoConstraint_L3A_L2F(Int_t ssdindex, Int_t csinum, Int_t csissdnum, Int_t l2fmulti, Int_t* l2fstrip, Int_t* l2fssdnum);
  Bool_t   IsGeoConstraint_L3A_L2F(Int_t ssdindex, Int_t csimulti, Int_t* csinum, Int_t* csissdnum, Int_t l2fstrip, Int_t l2fssdnum);
  Bool_t   IsGeoConstraint_L3A_L2F(Int_t ssdindex, Int_t csimulti, Int_t* csinum, Int_t* csissdnum, Int_t l2fmulti, Int_t* l2fstrip, Int_t* l2fssdnum);
  // L2B_L1S 几何约束
  Bool_t   IsGeoConstraint_L2B_L1S(Int_t stripl2b, Int_t stripl1s);
  Bool_t   IsGeoConstraint_L2B_L1S(Int_t stripl2b, Int_t stripl1s, Int_t deltastrip);
  Bool_t   IsGeoConstraint_L2B_L1S(Int_t ssdindex, Int_t l2bstrip, Int_t l2bssdnum, Int_t l1smulti, Int_t* l1sstrip, Int_t* l1sssdnum);
  Bool_t   IsGeoConstraint_L2B_L1S(Int_t ssdindex, Int_t l2bmulti, Int_t* l2bstrip, Int_t* l2bssdnum, Int_t l1sstrip, Int_t l1sssdnum);
  Bool_t   IsGeoConstraint_L2B_L1S(Int_t ssdindex, Int_t l2bmulti, Int_t* l2bstrip, Int_t* l2bssdnum, Int_t l1smulti, Int_t* l1sstrip, Int_t* l1sssdnum);
  // L2B_L2F 能量约束
  Bool_t   IsEneConstraint_L2B_L2F(Double_t El2b, Double_t El2f, Double_t ErrRatio);
  Bool_t   IsEneConstraint_L2B_L2F(Int_t ssdindex, Double_t El2b, Int_t l2bssdnum, Int_t l2fmulti, Double_t* El2f, Int_t* l2fssdnum, Double_t ErrRatio);
  Bool_t   IsEneConstraint_L2B_L2F(Int_t ssdindex, Int_t l2bmulti, Double_t* El2b, Int_t* l2bssdnum, Double_t El2f, Int_t l2fssdnum, Double_t ErrRatio);
  Bool_t   IsEneConstraint_L2B_L2F(Int_t ssdindex, Int_t l2bmulti, Double_t* El2b, Int_t* l2bssdnum, Int_t l2fmulti, Double_t* El2f, Int_t* l2fssdnum, Double_t ErrRatio);
  Bool_t   IsEneConstraint_L2B_L2F(Int_t ssdindex, Int_t l2bmulti, Double_t* El2b, Int_t* l2bssdnum, Int_t l2fmulti, Double_t* El2f, Int_t* l2fssdnum);
  Bool_t   IsEneConstraint_L2B_L2F(Int_t ssdindex, Double_t El2b, Double_t El2f);

  // 对于能穿透 L2 的粒子, 通过 LISE 计算给出 L1, L2 的能损比例, 作为新的约束条件
  Bool_t   IsEneConstraint_L1S_L2F(Int_t ssdindex, Double_t El1, Double_t El2);
  Bool_t   IsEneConstraint_L1S_L2F(Int_t ssdindex, Int_t l1smulti, Double_t* El1s, Int_t* l1sssdnum, Int_t l2fmulti, Double_t* El2f, Int_t* l2fssdnum);

  // 根据 L2F, L2B 的位置, 确定对应的 CsI 位置
  Int_t   GetCsINumFromPixel(Int_t stripl2f, Int_t stripl2b); 

  void     EstimateLayerMulti(Int_t firstrun, Int_t lastrun);
  void     GetPunchThroughEnergiesOfLCPs();

private:
  CSHINESSDCalibratedData fCSHINESSDCalibratedData;
  EnergyLossModule        fLISEModule;
};

#endif
