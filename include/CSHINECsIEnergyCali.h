#ifndef CSHINECSIENERGYCALI_H
#define CSHINECSIENERGYCALI_H

#include "../include/CSHINEParticleIdentification.h"


//******************************************************************************
class CSHINECsIEnergyCali
{
public:
  CSHINECsIEnergyCali(Int_t firstrun, Int_t lastrun);
	 ~CSHINECsIEnergyCali();

 //________________________________________________
 // 采用 DEEFIT 方法获取 CsI 晶体的（ECh, EMeV）能量点
  void  GetDEEFITCsIEnergyPoints(); // 经过 DEEFIT_Func14 与 LISE++ 计算, 得到 CsI 晶体中的能量点 (ECh, EMeV)


 //__________________________________________________________
 // 采用传统的 GraphicalCut 与投影的方法获取 CsI 晶体的（ECh, EMeV）能量点



};

#endif
