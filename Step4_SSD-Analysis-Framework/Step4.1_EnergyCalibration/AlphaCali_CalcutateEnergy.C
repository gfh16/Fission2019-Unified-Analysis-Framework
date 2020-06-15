/////////////////////////////////////////////////////////////////////////////////
//                  ALPHA CALIBRATION CALCULATION ENERGY                       //
//                                                                             //
//    Use this macro to calculate the real incident energy of alpha to silicon,//
//    because this a 2-um aluminizing Mylar.                                   //
//                                                                             //
//    Run this macro : root -l AlphaCali_CalEnergy.cpp                         //
//                                                                             //
//    Author gfh                                                               //
//    Date Nov 11, 2019                                                        //
//                                                                             //
/////////////////////////////////////////////////////////////////////////////////

#include "../../include/EnergyLossModule.h"

Double_t Energy1[3] = {5.155, 5.143, 5.103};   // Unit: MeV
Double_t Ratio1 [3] = {0.730, 0.151, 0.115};   // branch ratio
Double_t Energy2[2] = {5.486, 5.443};
Double_t Ratio2 [2] = {0.852, 0.128};
Double_t Energy3[2] = {5.805, 5.763};
Double_t Ratio3 [2] = {0.767, 0.233};
Double_t E1 = Eaverage(Energy1, Ratio1, 3);
Double_t E2 = Eaverage(Energy2, Ratio2, 2);
Double_t E3 = Eaverage(Energy3, Ratio3, 2);


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
void AlphaCali_CalculateEnergy()
{
  EnergyLossModule LISEModule;

  Double_t Ealpha[3] = {E1, E2, E3};  // MeV
  Double_t MylarThickness = 2.0;      // um
  Double_t AlThickness    = 0.06;      // um

  std::string pathCalculationResultsOut("output/SSD_AlphaEnergies.dat");

  ofstream FileOut(pathCalculationResultsOut.c_str());
  FileOut<<"* SSDNum"<<setw(7)<<"CHNum"<<setw(7)<<"E1"<<setw(10)<<"E2"<<setw(10)<<"E3"<<"  "<<"(MeV)\n";

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
      FileOut<<"    "<<SSDNum <<"     "<<CHNum<<"    "
             <<Eresidual2[0]<<"    "
             <<Eresidual2[1]<<"    "
             <<Eresidual2[2]<<endl;
    }
  }
  FileOut.close();
  return;
}
