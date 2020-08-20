////////////////////////////////////////////////////////////////////////////////
//
//   手动给 CsI 添加 Cut 值
//   CsI 无法单独刻度, 因此无法通过拟合得出 pedestal
//   这里手动给定一个 Cut 值, 且认为每块 CsI 的 Cut 值都一样
//
////////////////////////////////////////////////////////////////////////////////

#include "../../include/shared.h"

//______________________________________________________________________________
void CsICali_GenerateEChCut()
{
  Double_t echcut = ECSICHCUT;

  std::string pathDataOut("../Step4.1_EnergyCalibration/output/SSD_L3A_EChCut.dat");

  ofstream fileout(pathDataOut.c_str());
  fileout<<"* SSDNum"<<setw(10)<<"ChNum"<<setw(15)<<"CsIEChCut\n";

  for (Int_t i=0; i<4; i++)
  {
    for (Int_t j=0; j<9; j++)
    {
      fileout<<setw(5)<<i<<setw(10)<<j<<setw(13)<<echcut<<endl;
    }
  }
}
