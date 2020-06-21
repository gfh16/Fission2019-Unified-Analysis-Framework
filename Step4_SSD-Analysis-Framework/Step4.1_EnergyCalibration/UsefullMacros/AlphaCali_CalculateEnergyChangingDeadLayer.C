/////////////////////////////////////////////////////////////////////////////////
//                  ALPHA CALIBRATION CALCULATION ENERGY
//
//    Use this macro to calculate the real incident energy of alpha to silicon,
//    because this a 2-um aluminizing Mylar.
//
//    由于硅条探测器表面也有死层, 厚度未知, 现在想检测死层对刻度结果的影响, 思路是：
//       1. 改变死层的厚度, 计算 alpha 粒子的剩余能量作为在硅条中的能损
//       2. 为了方便起见, 通过改变 Mylar 膜中铝的厚度来等效代替硅条中死层的厚度
//       3. 由于死层是一个固定值, alpha 的能损也是固定的, 理论上: 改变 Al 膜厚度,
//          拟合效果最好的情况, 认为是充分考虑了死层的影响
//
//    对于 alpha 的能量: 3 alpha 峰分别为 Pu239, Am241, Cm244, 每一种放射源都有 2-3
//    个不同你能量的 alpha, 这里按照分支比取其平均值
//
//    Author gfh
//    Date March 27, 2020
//
/////////////////////////////////////////////////////////////////////////////////

#include "include/EnergyLossModule.h"



//______________________________________________________________________________
void ToGenerateFileOutHead(ofstream& FileOut);

void ToWriteFileOut(ofstream& FileOut, const Double_t Energies[], const Int_t size);

//______________________________________________________________________________
// 定义一个函数,用于计算两个数组元素乘积的求和
Double_t Eaverage(double *arr1, double *arr2, int size)
{
  Double_t Esum = 0; // energy
  Double_t Rsum = 0; // energy branch ratio
  Double_t average = 0;
  for(Int_t i=0;i<size;i++)
  {
    Esum += arr1[i]*arr2[i];
    Rsum += arr2[i];
  }
  average = Esum/Rsum;
  return average;
}

//______________________________________________________________________________
// alpha 能量数据来源于: 刘运祚 《常用放射性核素衰变纲图（1982）》
Double_t Energy1[3] = {5.155, 5.143, 5.103};   // Unit: MeV    Pu239
Double_t Ratio1 [3] = {0.730, 0.151, 0.115};   // branch ratio
Double_t Energy2[2] = {5.486, 5.443};          // Unit: MeV    Am241
Double_t Ratio2 [2] = {0.852, 0.128};
Double_t Energy3[2] = {5.805, 5.763};          // Unit: MeV    Cm244
Double_t Ratio3 [2] = {0.767, 0.233};
Double_t E1 = Eaverage(Energy1, Ratio1, 3);
Double_t E2 = Eaverage(Energy2, Ratio2, 2);
Double_t E3 = Eaverage(Energy3, Ratio3, 2);

const Int_t SSDNum = 4;
const Int_t CHNum = 16;
const Int_t PeaksNum = 3;
Double_t Ealpha[PeaksNum] = {E1, E2, E3};  // MeV


Double_t MylarThickness = 2.0;      // um
Double_t AlThicknessStep = 0.5;    // AlThickness = AlThicknessStep * j;
Int_t NumofAlThickness = 10;


//______________________________________________________________________________
void AlphaCali_CalculateEnergyChangingDeadLayer()
{
  EnergyLossModule LISEModule;

  std::string pathCalculationResultOutE1("output/SSD_AlphaE1_ChangingDeadLayer.dat");
  std::string pathCalculationResultOutE2("output/SSD_AlphaE2_ChangingDeadLayer.dat");
  std::string pathCalculationResultOutE3("output/SSD_AlphaE3_ChangingDeadLayer.dat");

  ofstream FileOutE1(pathCalculationResultOutE1.c_str());
  ofstream FileOutE2(pathCalculationResultOutE2.c_str());
  ofstream FileOutE3(pathCalculationResultOutE3.c_str());

  Double_t Eloss1[PeaksNum];
  Double_t Eresidual1[PeaksNum];
  Double_t Eloss2[PeaksNum][NumofAlThickness];
  Double_t Eresidual2[PeaksNum][NumofAlThickness];
  Double_t AlThickness[NumofAlThickness];
  for (Int_t i=0; i<3; i++)
  {
    Eloss1[i] = LISEModule.GetEnergyLoss(2,4,Ealpha[i],"Mylar",MylarThickness);
    Eresidual1[i] = Ealpha[i] - Eloss1[i];
    for (Int_t j=0; j<NumofAlThickness; j++)
    {
      AlThickness[j] = AlThicknessStep * j;
      Eloss2[i][j]     = LISEModule.GetEnergyLoss(2,4,Eresidual1[i],"Al",AlThickness[j]);
      Eresidual2[i][j] = Eresidual1[i] - Eloss2[i][j];  //alpha粒子穿过Mylar膜之后的能量
      cout<<"E_Incident = "<< Ealpha[i] <<"   "<<"E_residual = "<< Eresidual2[i][j]<<"   "
          <<"ELoss_Total = "<<Ealpha[i]-Eresidual2[i][j]<<"   "<<"AlThickness ="<< AlThickness[j]<< endl;
    }
  }

  ToGenerateFileOutHead(FileOutE1);
  ToGenerateFileOutHead(FileOutE2);
  ToGenerateFileOutHead(FileOutE3);

  ToWriteFileOut(FileOutE1,Eresidual2[0],NumofAlThickness);
  ToWriteFileOut(FileOutE2,Eresidual2[1],NumofAlThickness);
  ToWriteFileOut(FileOutE3,Eresidual2[2],NumofAlThickness);

  FileOutE1.close();
  FileOutE2.close();
  FileOutE3.close();

  return;
}


//______________________________________________________________________________
void ToGenerateFileOutHead(ofstream& FileOut)
{
  FileOut<<"* Calculate the alpha peak energies, changing the thickness of the dead layer.\n";
  FileOut<<Form("* AlThickness = 0.05 + %.2f * i (um), i=0,1,2,...\n", AlThicknessStep);
  FileOut<<"* Thres alpha peaks: E1=5.14718, E2=5.48038, E3=5.79521; E(i) = energy after mylar.\n";
  FileOut<<"* SSDNum"<<setw(7)<<"CHNum"<<setw(7)<<"E(i=0)"<<setw(10)<<"E(i=1)"<<setw(10)<<"E(i=2)"<<setw(10)
         <<"E(i=3)"<<setw(10)<<"E(i=4)"<<setw(10)<<"E(i=5)"<<setw(10)<<"E(i=6)"<<setw(10)<<"E(i=7)"<<setw(10)
         <<"E(i=8)"<<setw(10)<<"E(i=9)"<<endl;
}


//______________________________________________________________________________
void ToWriteFileOut(ofstream& FileOut, const Double_t Energies[], const Int_t size)
{
  for (Int_t ssdnum=0; ssdnum<SSDNum; ssdnum++)
  {
    for (Int_t chnum=0; chnum<CHNum; chnum++)
    {
      FileOut<<ssdnum<<setw(10)<<chnum<<setw(10);
      for (Int_t index=0; index<size; index++)
      {
        FileOut<<Energies[index]<<setw(10);
      }
      FileOut<<endl;
    }
  }
}
