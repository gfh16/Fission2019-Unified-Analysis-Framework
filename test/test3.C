#include "../include/CSHINESSDCalibratedData.h"


//___________________________________________________
void test3()
{
  CSHINESSDCalibratedData cali;

  Double_t* pede      = cali.GetSiEChPedestals("L1S");
  Double_t* echcut    = cali.GetSiEChCut("L1S", 5);
  Double_t* slope     = cali.GetSiCaliSlopeDLC("L1S");
  Double_t* inter     = cali.GetSiCaliInterceptDLC("L1S");
  Double_t* emevcut   = cali.GetSiEMeVCutDLC("L1S", 5);
  Double_t* echcsi    = cali.GetCsIEChCut("L3A");

 for (Int_t i=0; i<4; i++)
 {
   for (Int_t j=0; j<16; j++)
   {
     cout<<setw(10)<<i<<setw(10)<<j<<setw(10)<<pede[i*16+j]
         <<setw(15)<<echcut[i*16+j]<<setw(15)<<slope[i*16+j]
         <<setw(15)<<inter[i*16+j] <<setw(15)<<emevcut[i*16+j]
         <<endl;
   }
 }

 for (Int_t i=0; i<4; i++)
 {
   for (Int_t j=0; j<9; j++)
   {
     cout<<setw(10)<<i<<setw(10)<<j<<setw(10)<<echcsi[i*9+j]<<endl;
   }
 }

}
