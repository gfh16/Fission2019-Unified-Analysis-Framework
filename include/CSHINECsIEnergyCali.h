#ifndef CSHINECSIENERGYCALI_H
#define CSHINECSIENERGYCALI_H

#include "../include/CSHINEParticleIdentification.h"
#include "../include/EnergyLossModule.h"
#include "../include/shared.h"


//******************************************************************************
class CSHINECsIEnergyCali
{
public:
  CSHINECsIEnergyCali(Int_t firstrun, Int_t lastrun);
	 ~CSHINECsIEnergyCali();

 //________________________________________________
 // 采用 DEEFIT 方法获取 CsI 晶体的（ECh, EMeV）能量点
  void       GetDEEFITCsIEnergyPoints(); // 经过 DEEFIT_Func14 与 LISE++ 计算, 得到 CsI 晶体中的能量点 (ECh, EMeV)
  Double_t   EnergyDepositedInCsI(Int_t Z, Int_t A, Double_t ELoss, Double_t SiThickness, Double_t CsIMylarThickness);

 //__________________________________________________________________
 // 采用传统的 GraphicalCut 与投影的方法获取 CsI 晶体的（ECh, EMeV）能量点
  void       DoGraphicalCut();
  void       GetProjectionCsIEnergPoints();

 //_______________________________________
 // 对提取的数据点进行拟合


private:
  Int_t              fFirstRun;
  Int_t              fLastRun;
  EnergyLossModule   fLISEModule;
  CSHINEDEEFITPID   *fDeefit;
  DEEFITParticle     fDEEFITParticle;

  Double_t           SiThickness[4]    = {1010, 1008, 526, 306}; // 单位: 微米
  Double_t           CsIMylarThickness = 2.0;  // 单位: 微米
  Int_t              fNParticles       = 19;
	Int_t              fParticle[19][2]  = { {1,1},{1,2},{1,3},{2,3},{2,4},{2,6},{3,6},{3,7},
	                                         {3,8},{3,9},{4,7},{4,9},{4,10},{5,10},{5,11},{5,12},
														               {6,12},{6,13},{6,14}
												                 };
};

#endif
