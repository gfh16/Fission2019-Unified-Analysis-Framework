#ifndef CSHINEPARTICLEIDENTIFICATIONSIMPLIFIED_H
#define CSHINEPARTICLEIDENTIFICATIONSIMPLIFIED_H

#include "shared.h"
#include "ReadFileModule.h"

#include <vector>


//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
//  实际数据分析中, 使用 "拉直法" 进行粒子鉴别
//  CSHINEParticleIdentificationSimplified 的所有成员函数都复制于
//  CSHINEParticleIdentification. 为了增强代码的可读性,这是简化版,仅仅包含了实际分析
//  中用到的功能
//
// P.S. 关于粒子鉴别的详细情况,请参考 CSHINEParticleIdentification
// gfh, 2021-08
//______________________________________________________________________________
class CSHINEParticleIdentificationSimplified
{
public:
  CSHINEParticleIdentificationSimplified();
  ~CSHINEParticleIdentificationSimplified();

  void       PrintUsage();

public:
  Double_t   L2L3_GetPIDNumFromExpData(Int_t ssdindex, Int_t csiindex, Double_t EStoped, Double_t dE, Int_t icharge, Int_t imass);
  Double_t   L1L2_GetPIDNumFromExpData(Int_t ssdindex, Double_t EStoped, Double_t dE, Int_t icharge, Int_t imass);
  Double_t   DoCalcdEMeV(Double_t ex, Double_t* parlist, Int_t fitparsum);
  Double_t   StdPIDNum(Int_t Z_charge, Int_t A_mass);
  Double_t   GetMassFromPIDNum(Double_t pidnum, Int_t charge);
  Int_t      GetZoneOfPixel(Int_t stripb, Int_t stripf, Int_t ZoneLength) const;

private:
  const Int_t         NFITPARS  = 8;

  std::vector<Int_t>  L2L3_Charge[36];
  std::vector<Int_t>  L2L3_Mass[36];
  Double_t            L2L3_DEEFitPars[36][20][8];

  std::vector<Int_t>  L1L2_Charge[4];
  std::vector<Int_t>  L1L2_Mass[4];
  Double_t            L1L2_DEEFitPars[4][40][8];

private:
  void                Init_L2L3StraighteningFitPars();
  void                Init_L1L2StraighteningFitPars();
  std::string         pathL2L3StraighteningFitPars = Form("%sdata_ForPhysicsEvent/L2L3_StraighteningFitPars.dat",PATHDATAFOLDER);
  std::string         pathL1L2StraighteningFitPars = Form("%sdata_ForPhysicsEvent/L1L2_StraighteningFitPars.dat",PATHDATAFOLDER);
};
#endif
