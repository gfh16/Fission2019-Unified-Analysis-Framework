#include "../include/CSHINEBuildEvent.h"
#include "../include/CSHINEEvent.h"
#include "../include/ReadFileModule.h"
#include "../include/TimeAndPercentage.h"
#include "../include/CSHINESSDCalibratedData.h"


Int_t FirstRun = 200;
Int_t LastRun  = 201;
Int_t NumSigma = 10;

//______________________________________________________________________________
void test6()
{
  TimeAndPercentage time;
  time.GetBeginTime();

  std::string L1STag("L1S");
  std::string L2FTag("L2F");
  std::string L2BTag("L2B");
  std::string L3ATag("L3A");
  std::string pathRootInputFolder("/home/sea/Fission2019_Data/MapRoot/");

  Double_t* echl1scut;
  Double_t* echl2fcut;
  Double_t* echl2bcut;
  Double_t* echcsicut;
  Double_t* pedel1s;
  Double_t* pedel2f;
  Double_t* pedel2b;
  Double_t* kl1s;
  Double_t* kl2f;
  Double_t* kl2b;
  Double_t* hl1s;
  Double_t* hl2f;
  Double_t* hl2b;
  Double_t* emevcutl1s;
  Double_t* emevcutl2f;
  Double_t* emevcutl2b;


  CSHINESSDCalibratedData calidata;
  echl1scut  =  calidata.GetSiEChCut (L1STag.c_str(), NUM_SIGMA);
  echl2fcut  =  calidata.GetSiEChCut (L2FTag.c_str(), NUM_SIGMA);
  echl2bcut  =  calidata.GetSiEChCut (L2BTag.c_str(), NUM_SIGMA);
  echcsicut  =  calidata.GetCsIEChCut(L3ATag.c_str());

  pedel1s    =  calidata.GetSiEChPedestals(L1STag.c_str());
  pedel2f    =  calidata.GetSiEChPedestals(L2FTag.c_str());
  pedel2b    =  calidata.GetSiEChPedestals(L2BTag.c_str());

  kl1s       =  calidata.GetSiCaliSlope(L1STag.c_str());
  kl2f       =  calidata.GetSiCaliSlope(L2FTag.c_str());
  kl2b       =  calidata.GetSiCaliSlope(L2BTag.c_str());
  hl1s       =  calidata.GetSiCaliIntercept(L1STag.c_str());
  hl2f       =  calidata.GetSiCaliIntercept(L2FTag.c_str());
  hl2b       =  calidata.GetSiCaliIntercept(L2BTag.c_str());

  emevcutl1s =  calidata.GetSiEMeVCut(L1STag.c_str(), NUM_SIGMA);
  emevcutl2f =  calidata.GetSiEMeVCut(L2FTag.c_str(), NUM_SIGMA);
  emevcutl2b =  calidata.GetSiEMeVCut(L2BTag.c_str(), NUM_SIGMA);


  for (Int_t i=0; i<NUM_SSD; i++)
  {
    for (Int_t j=0; j<NUM_STRIP; j++)
    {
      cout<<i<<setw(10)<<j
          <<setw(10)<<pedel1s[i*NUM_STRIP+j]
          <<setw(10)<<pedel2f[i*NUM_STRIP+j]
          <<setw(10)<<pedel2b[i*NUM_STRIP+j]
          <<setw(10)<<echl1scut[i*NUM_STRIP+j]
          <<setw(10)<<echl2fcut[i*NUM_STRIP+j]
          <<setw(10)<<echl2bcut[i*NUM_STRIP+j]
          <<setw(10)<<(j<9 ? echcsicut[i*NUM_CSI+j] : 0)
          <<setw(15)<<kl1s[i*NUM_STRIP+j]<<setw(10)<<kl2f[i*NUM_STRIP+j]<<setw(10)<<kl2b[i*NUM_STRIP+j]
          <<setw(10)<<hl1s[i*NUM_STRIP+j]<<setw(10)<<hl2f[i*NUM_STRIP+j]<<setw(10)<<hl2b[i*NUM_STRIP+j]
          <<setw(10)<<emevcutl1s[i*NUM_STRIP+j]
          <<setw(10)<<emevcutl2f[i*NUM_STRIP+j]
          <<setw(10)<<emevcutl2b[i*NUM_STRIP+j]
          <<endl;
     }
   }

  time.GetEndTime();
  time.GetRunTime();
}
