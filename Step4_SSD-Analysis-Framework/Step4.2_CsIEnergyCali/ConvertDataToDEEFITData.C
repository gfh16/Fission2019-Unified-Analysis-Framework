////////////////////////////////////////////////////////////////////////////////
//
//    DEEFIT Root data 数据格式有特殊的要求:
//    ** tree 中只有三个 branch
//       h1: tree 的名字
//        numtel:  CsI 编号,
//        desipgf: Si中能损(MeV)
//        fastpg:  CsI中的能量沉积(Ch)
//
//    ** 数据类型:
//        numtel/s     --> numtel/i
//        desipgf/f    --> desimev/d
//        fastpg/s     --> ecsich/i
//
//   现在要做的是: (只考虑 one-hit 事件)
//   1. 给 CsI 设定一个 Cut 值, 当 ECsI_CH > Cut 时, 认为 CsI 中是一个有效的 hit
//   2. 利用硅条的能量刻度曲线, 计算粒子在硅中的能量 ESi_MeV
//   3. 创建 tree h1，有三个 branch:
//      (1) numtel/s  : 从 0 - 35, 共 4 * 9 = 36 块 CsI
//      (2) desimev/d : ESi_MeV
//      (3) ecsich/d  : ECsI_CH
//    需要注意 branch 的数据类型!
//
////////////////////////////////////////////////////////////////////////////////

#include "../../include/EnergyLossModule.h"
#include "../../inlcude/ReadFileModule.h"

Int_t SSDNum   = 4;
Int_t CHNum    = 16;
Int_t CsINum   = 9;

Int_t FirstRun = 200;
Int_t LastRun  = 220;

Int_t ParNum   = 2;
Int_t SigmaNum = 5;
Int_t NBinsX   = 4096;
Int_t NBinsY   = 4096;
Int_t HitMultiCut = 1;


//______________________________________________________________________________
void ConvertDataToDEEFITData()
{

  std::string pathExpDataFolder("/home/sea//Fission2019_Data/MapRoot/");

  std::string L2FTag("L2F");
  std::string L2BTag("L2B");
  std::string L3ATag("L3A");

  Int_t SSD_L2F_HitMulti[SSDNum];
  Int_t SSD_L2B_HitMulti[SSDNum];

  Int_t SSD_L2F_E[SSDNum][CHNum];
  Int_t SSD_L2B_E[SSDNum][CHNum];
  Int_t SSD_L3A_E[SSDNum][CsINum];

  EnergyLossModule lisecalculator;
  ReadFileModule   readfile;






  // Load experimental data
  TChain* myData = new TChain("KrPb");
  for (Int_t i=0; i< (LastRun+1); i++)
  {
    myData->Add(Form("%sMapFission2019_Kr_Pb%04d.root", pathExpDataFolder.c_str(), i));
  }
  myData->SetBranchStatus("*",false); // 先关闭所有的 branch

  for (Int_t i=0; i<SSDNum; i++)
  {

  }

}
