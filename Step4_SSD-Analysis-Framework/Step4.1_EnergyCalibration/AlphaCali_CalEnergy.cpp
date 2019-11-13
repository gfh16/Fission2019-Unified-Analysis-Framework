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

#include "include/EnergyLossModule.h"

//______________________________________________________________________________
// 定义一个函数,用于计算两个数组元素乘积的求和
double Eaverage(double *arr1, double *arr2, int size)
{
  double Esum = 0;
  double Rsum = 0;
  double average = 0;
  for(int i=0;i<size;i++)
  {
    Esum += arr1[i]*arr2[i];
    Rsum += arr2[i];
  }
  average = Esum/Rsum;
  return average;
}

double Energy1[3] = {5.155, 5.143, 5.103};   // three alpha energies of 239Pu
double Ratio1 [3] = {0.730, 0.151, 0.115};   // branch ratio
double Energy2[2] = {5.486, 5.443};   // two alpha energies of 241Am
double Ratio2 [2] = {0.852, 0.128};
double Energy3[2] = {5.805, 5.763};   // two alpha energies of 244Cm
double Ratio3 [2] = {0.767, 0.233};
double E1 = Eaverage(Energy1, Ratio1, 3);
double E2 = Eaverage(Energy2, Ratio2, 2);
double E3 = Eaverage(Energy3, Ratio3, 2);

//______________________________________________________________________________
void AlphaCali_CalEnergy()
{
  EnergyLossModule LISEModule;

  double Ealpha[3] = {E1, E2, E3};  // MeV
  double MylarThickness = 2.0;      // um
  double AlThickness    = 0.06;      // um

  ofstream FileOut("output/SSD_Alpha_Energy.dat");
  FileOut<<"* SSDNum"<<setw(7)<<"CHNum"<<setw(7)<<"E1"<<setw(10)<<"E2"<<setw(10)<<"E3"<<"  "<<"(MeV)\n";

  double Eloss1[3];
  double Eloss2[3];
  double Eresidual1[3];
  double Eresidual2[3];
  for(int i=0; i<3; i++)
  {
    Eloss1[i]     = LISEModule.GetEnergyLoss(2,4,Ealpha[i],"Mylar",MylarThickness);
    Eresidual1[i] = Ealpha[i] - Eloss1[i];
    Eloss2[i]     = LISEModule.GetEnergyLoss(2,4,Eresidual1[i],"Al",AlThickness);
    Eresidual2[i] = Eresidual1[i] - Eloss2[i];  //alpha粒子穿过Mylar膜之后的能量
    cout<<"E_Incident = "<< Ealpha[i] <<"  "<<"E_residual = "<< Eresidual2[i]<<"  "
        <<"ELoss_Total = "<<Ealpha[i]-Eresidual2[i]<<endl;
  }
  for(int SSDNum=0; SSDNum<4; SSDNum++)
  {
    for(int CHNum=0; CHNum<16; CHNum++)
    {
      FileOut<<"    "<<SSDNum <<"     "<<setw(2)<<setfill('0')<<CHNum<<"    "
             <<Eresidual2[0]<<"    "
             <<Eresidual2[1]<<"    "
             <<Eresidual2[2]<<endl;
    }
  }
  return;
}
