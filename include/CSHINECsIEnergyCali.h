#ifndef  CSHINECSIENERGYCALI_H
#define CSHINECSIENERGYCALI_H


//***********************************************************************************
// 定义 DEEFIT 的数据结构
struct DEEFITData
{
// DEEFIT 原始数据格式
  UShort_t numtel;  // number of csi crystals
  Float_t  desipgf;    // dE (MeV)
  UShort_t fastpg;  // ECsI (ADC Chs)
};


//***********************************************************************************
class CSHINECsIEnergyCali
{
public:
  CSHINECsIEnergyCali();
	 ~CSHINECsIEnergyCali();

  void GenerateDEEFITData(Int_t firstrun, Int_t lastrun);  // 借助DEEFIT 工具进行CsI能量刻度, 需要先将实验数据存成DEEFIT格式

private:
  DEEFITData   *fdeefitdata;

};





#endif
