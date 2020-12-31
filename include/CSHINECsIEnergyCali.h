#ifndef  CSHINECSIENERGYCALI_H
#define CSHINECSIENERGYCALI_H


//******************************************************************************
// 定义 DEEFIT 的数据结构
struct DEEFITTreeData
{
// DEEFIT 原始数据格式
  UShort_t numtel;  // number of csi crystals
  Float_t  desipgf; // dE (MeV)
  UShort_t fastpg;  // ECsI (ADC Chs)
};

// 定义粒子属性的数据结构
struct DEEFITParticle
{
  Double_t E, dE;
  Double_t Z, A; // charge and mass number, double type
};


//******************************************************************************
class CSHINECsIEnergyCali
{
public:
  CSHINECsIEnergyCali(Int_t firstrun, Int_t lastrun);
	 ~CSHINECsIEnergyCali();

 //________________________________________________
 // 采用 DEEFIT 方法获取 CsI 晶体的（ECh, EMeV）能量点
  void       GenerateDEEFITData(); // 借助DEEFIT 工具进行CsI能量刻度, 需要先将实验数据存成DEEFIT格式
  void       RunDEEFitCode(); // 运行 DEEFIT 程序，手动选点，对 dE(ECsI,Z,A) 进行拟合
  void       GetDEEFITCsIEnergyPoints(); // 经过 DEEFIT_Func14 与 LISE++ 计算, 得到 CsI 晶体中的能量点 (ECh, EMeV)
  Double_t   DEEFITFunc14(DEEFITParticle& p, Double_t* par);

 //__________________________________________________________
 // 采用传统的 GraphicalCut 与投影的方法获取 CsI 晶体的（ECh, EMeV）能量点

private:
  Int_t            fFirstRun;
  Int_t            fLastRun;
  DEEFITTreeData  *fdeefitdata;

};

#endif
