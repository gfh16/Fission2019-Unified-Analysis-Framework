#ifndef CSHINECSIENERGYCALISIMPLIFIED_H
#define CSHINECSIENERGYCALISIMPLIFIED_H

#include "../include/shared.h"
#include "../include/ReadFileModule.h"

#include "TSpline.h"
#include "TCutG.h"
#include "TF1.h"


//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
// 定义粒子属性的数据结构
struct MParticle
{
  Double_t E, dE;
  Double_t Z, A; // charge and mass number, double type
  Double_t e;    // Error
};
//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo


//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
//  这个类是为了对 CsI 能量刻度参数的调用而写,CSHINECsIEnergyCaliSimplified 所有的成员
//  函数都来自于 CSHINECsIEnergyCali 这个类, 这里仅仅摘取了必要的功能，以便阅读
//  CsI 能量刻度的方法请参考 CSHINECsIEnergyCali
//
// gfh, 2021-08
//___________________________________
class CSHINECsIEnergyCaliSimplified
{
public:
  CSHINECsIEnergyCaliSimplified();
	 ~CSHINECsIEnergyCaliSimplified();

  void       PrintUsage();

public:
 // CsI 能量拟合函数
  Double_t   DrawFunc_Hydrogen(Double_t* x, Double_t* par);  // fit Z=1
  Double_t   DrawFunc_HeavyIon(Double_t* x, Double_t* par);  // fit Z>=2
  Double_t   CsICaliChToMeV(Int_t ssdindex, Int_t csiindex, Int_t charge, Int_t mass, Double_t ECh) const;

private:
  std::vector<Int_t>      Z_InCsI[NUM_SSD*NUM_CSI];
  std::vector<Int_t>      A_InCsI[NUM_SSD*NUM_CSI];
  std::vector<Double_t>   CsIEnergyFitPars_Z1[NUM_SSD*NUM_CSI];
  std::vector<Double_t>   CsIEnergyFitPars_ZOver2[NUM_SSD*NUM_CSI];

  const Int_t             NFitPars_Z1 = 4;
  const Int_t             NFitPars_ZOver2 = 3;
  const Int_t             NPoints_ForInterpolation = 60;
  const Double_t          EnergyLowLimitsForInterpolation = 0.0;
  const Double_t          EnergyUpLimitsForInterpolation  = 300.0;

  TF1*                    fFuncCsIFitting[NUM_SSD*NUM_CSI][7][15];
  TSpline3*               fECsISpline[NUM_SSD*NUM_CSI][7][15];

  void                    Init_ZA_InCsI();
  void                    Init_fECsISpline();
  void                    Init_fFuncCsIFitting();
  std::string             pathGetZAInCsI = Form("%sdata_ForPhysicsEvent/L2L3_StraighteningFitPars.dat",PATHDATAFOLDER);
  std::string             pathCsIFitPars_Z1     = Form("%sdata_ForPhysicsEvent/DEEFITCsIEnergyFitPars_Z1.dat",PATHDATAFOLDER);
  std::string             pathCsIFitPars_ZOver2 = Form("%sdata_ForPhysicsEvent/DEEFITCsIEnergyFitPars_ZOver2.dat",PATHDATAFOLDER);
  ReadFileModule          readfile;
};

#endif
