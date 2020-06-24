/////////////////////////////////////////////////////////////////////////////////
//                  ALPHA CALIBRATION CALCULATION ENERGY
//
//    修正 alpha 的能量. 能量修正主要来源于两部分:
//     1. 硅条前面的镀铝 Mylar 膜: 2.0um Mylar + 0.06um Al
//     2. 硅条的死层
//    这里的计算先不考虑硅的死层，仅计算 alpha 在 镀铝 Mylar 膜中的能损
//
//    对于 alpha 的能量: 3 alpha 峰分别为 Pu239, Am241, Cm244, 每一种放射源都有 2-3
//    个不同你能量的 alpha, 这里按照分支比取其平均值
//
//
//    gfh, 2019-11-11
/////////////////////////////////////////////////////////////////////////////////

#include "../../include/EnergyLossModule.h"

//_______________________________________________________
Double_t Eaverage(double *arr1, double *arr2, int size);

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
Double_t Ealpha[3] = {E1, E2, E3};  // MeV
Double_t MylarThickness = 2.0;      // um
Double_t AlThickness    = 0.06;      // um



//______________________________________________________________________________
void AlphaCali_CalculateEnergy()
{
  std::string pathCalculationResultsOut("output/SSD_AlphaEnergies.dat");

  ofstream FileOut(pathCalculationResultsOut.c_str());
  FileOut<<setw(5)<<"* MylarThickness = 2.0um, AlThickness = 0.06um \n";
  FileOut<<setw(5)<<"* SSDNum"<<setw(7)<<"CHNum"<<setw(15)<<"E1_Residual"<<setw(12)<<"E2_Residual"
         <<setw(15)<<"E3_Residual"<<setw(7)<<"E1"<<setw(12)<<"E2"<<setw(15)<<"E3"
         <<setw(15)<<"E1_Loss"<<setw(12)<<"E2_Loss"<<setw(12)<<"E3_Loss"<<setw(5)<<"(MeV)\n";

  EnergyLossModule LISEModule;

  Double_t Eloss1[3];
  Double_t Eloss2[3];
  Double_t Eresidual1[3];
  Double_t Eresidual2[3];

  for(Int_t i=0; i<3; i++)
  {
    Eloss1[i]     = LISEModule.GetEnergyLoss(2,4,Ealpha[i],"Mylar",MylarThickness);
    Eresidual1[i] = Ealpha[i] - Eloss1[i];
    Eloss2[i]     = LISEModule.GetEnergyLoss(2,4,Eresidual1[i],"Al",AlThickness);
    Eresidual2[i] = Eresidual1[i] - Eloss2[i];  //alpha粒子穿过Mylar膜之后的能量
    cout<<"E_Incident = "<< Ealpha[i] <<"  "<<"E_residual = "<< Eresidual2[i]<<"  "
        <<"ELoss_Total = "<<Ealpha[i]-Eresidual2[i]<<endl;
  }

  for(Int_t SSDNum=0; SSDNum<4; SSDNum++)
  {
    for(Int_t CHNum=0; CHNum<16; CHNum++)
    {
      FileOut<<setw(5)<<SSDNum <<setw(7)<<CHNum
             <<setw(15)<<Eresidual2[0]<<setw(13)<<Eresidual2[1]<<setw(13)<<Eresidual2[2]
             <<setw(13)<<E1<<setw(13)<<E2<<setw(13)<<E3
             <<setw(13)<<Ealpha[0]-Eresidual2[0]
             <<setw(13)<<Ealpha[1]-Eresidual2[1]
             <<setw(13)<<Ealpha[2]-Eresidual2[2]<<endl;
    }
  }
  FileOut.close();
  return;
}


//______________________________________________________________________________
// 定义一个函数,用于计算两个数组元素乘积的求和
Double_t Eaverage(Double_t* arr1, Double_t* arr2, Int_t size)
{
  Double_t Esum = 0; // energy
  Double_t Rsum = 0; // energy branch ratio
  Double_t average = 0;
  for(Int_t i=0; i<size; i++)
  {
    Esum += arr1[i]*arr2[i];
    Rsum += arr2[i];
  }
  average = Esum/Rsum;
  return average;
}
