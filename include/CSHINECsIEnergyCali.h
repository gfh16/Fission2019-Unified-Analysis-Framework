#ifndef CSHINECSIENERGYCALI_H
#define CSHINECSIENERGYCALI_H

#include "../include/CSHINEParticleIdentification.h"
#include "../include/EnergyLossModule.h"
#include "../include/shared.h"
#include "../include/ReadFileModule.h"

#include "TSpline.h"
#include "TCutG.h"
#include "TGraph2D.h"
#include "TF2.h"
#include "TF1.h"

#include <string>
#include <iostream>
#include <fstream>

//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
class CSHINECsIEnergyCali
{
public:
  CSHINECsIEnergyCali();
  CSHINECsIEnergyCali(Int_t firstrun, Int_t lastrun);
	 ~CSHINECsIEnergyCali();

  void PrintUsage();


 //________________________________________________
 // 采用 DEEFIT 方法获取 CsI 晶体的（ECh, EMeV）能量点
  void       GetDEEFITCsIEnergyPoints(); // 经过 DEEFIT_Func14 与 LISE++ 计算, 得到 CsI 晶体中的能量点 (ECh, EMeV)
  void       DEEFITDrawAndFit_Z1();
  void       DEEFITDrawAndFit_ZOver2();

 // 采用 “拉直法” 拟合公式获取 CsI 晶体的 ECh, EMeV）能量点
  void       GetStraighteningCsIEnergyPoints();
  void       StraighteningDrawAndFit_Z1();
  void       StraighteningDrawAndFit_ZOver2();


  Double_t   EnergyDepositedInCsI(Int_t Z, Int_t A, Double_t ELoss, Double_t SiThickness, Double_t CsIMylarThickness);

 //__________________________________________________________________
 // 采用传统的 GraphicalCut 与投影的方法获取 CsI 晶体的（ECh, EMeV）能量点
  void       DoGraphicalCut();
  void       GetProjectionCsIEnergPoints();


 //____________________________________
 // functios for fitting and drawing
 // fit Z=1
  Double_t   FitFunc_Hydrogen(Double_t* x, Double_t* par); // 自定义函数，NIMA 929,162(2019)
  Double_t   DrawFunc_Hydrogen(Double_t* x, Double_t* par);
  Double_t   Func4_Hydrogen(DEEFITParticle& p, Double_t* par);

 // fit Z>=2
  Double_t   FitFunc_HeavyIon(Double_t* x, Double_t* par);
  Double_t   DrawFunc_HeavyIon(Double_t* x, Double_t* par);
  Double_t   Func3_HeavyIon(DEEFITParticle& p, Double_t* par);

 // check fit results
  void       CheckCsIEnergyCaliResults();


public:
  // CsI 能量刻度: ECh->EMeV
  Double_t   CsICaliChToMeV(Int_t ssdindex, Int_t csiindex, Int_t charge, Int_t mass, Double_t ECh) const;


private:
  Int_t                   fFirstRun;
  Int_t                   fLastRun;
  EnergyLossModule        fLISEModule;
  CSHINEDEEFITPID        *fDeefit;
  DEEFITParticle          fDEEFITParticle;
  CSHINEStraighteningPID  fStraightening;
  TimeAndPercentage       timeper;

  Double_t                SiThickness[4]    = {1010, 1008, 526, 306}; // 单位: 微米
  const Double_t          CsIMylarThickness = 2.0;  // 单位: 微米
  const Int_t             fMAXCHARGE        = 10;
  const Int_t             fMAXPARTICLE      = 30;
	const Int_t             fParticle[19][2]  = { {1,1},{1,2},{1,3},{2,3},{2,4},{2,6},{3,6},{3,7},
	                                              {3,8},{3,9},{4,7},{4,9},{4,10},{5,10},{5,11},{5,12},
														                    {6,12},{6,13},{6,14}
												                      };
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

  ReadFileModule          readfile;
  void                    Init_ZA_InCsI();
  void                    Init_fECsISpline();
  void                    Init_fFuncCsIFitting();
  std::string             pathGetZAInCsI = Form("%sdata_PID/L2L3_StraighteningFitPars.dat",PATHDATAFOLDER);
  std::string             pathCsIFitPars_Z1     = Form("%sdata_CsIEnergyCali/DEEFITCsIEnergyFitPars_Z1.dat",PATHDATAFOLDER);
  std::string             pathCsIFitPars_ZOver2 = Form("%sdata_CsIEnergyCali/DEEFITCsIEnergyFitPars_ZOver2.dat",PATHDATAFOLDER);

};

#endif
