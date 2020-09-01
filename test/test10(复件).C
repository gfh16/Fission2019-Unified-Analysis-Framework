#include "../include/TimeAndPercentage.h"
#include "../include/CSHINEHitPatternRecognition.h"
#include "../include/CSHINESSDCalibratedData.h"
#include "../include/shared.h"


Int_t FirstRun = 200;
Int_t LastRun  = 201;

const Int_t NPoints       =  10;
Int_t SigmaIndex[NPoints] =  {0,1,3,5,7,10,12,15,17,20}; // 取多个 sigma 值

Int_t MultiCut = 2;


//______________________________________________________________________________
Double_t EventRatioWithMultiCut(Int_t ssdindex, const char* layertag, Int_t effentries, Int_t entriswithmulticut);
void     AddNumOfCount(Int_t multi, Int_t& effentries, Int_t& entrieswithmulticut);

//______________________________________________________________________________
void test10()
{
  TimeAndPercentage time;
  time.GetBeginTime();

  std::string L1STag("L1S");
  std::string L2FTag("L2F");
  std::string L2BTag("L2B");
  std::string L3ATag("L3A");

  std::string pathRootInputFolder("/home/sea/Fission2019_Data/MapRoot/");

  Int_t L1SMulti[NUM_SSD][NPoints]  =  {{0}};
  Int_t L2FMulti[NUM_SSD][NPoints]  =  {{0}};
  Int_t L2BMulti[NUM_SSD][NPoints]  =  {{0}};
  Int_t L3AMulti[NUM_SSD][NPoints]  =  {{0}};

  Int_t L1SNumOfEffEntries[NUM_SSD][NPoints] = {{0}};
  Int_t L2FNumOfEffEntries[NUM_SSD][NPoints] = {{0}};
  Int_t L2BNumOfEffEntries[NUM_SSD][NPoints] = {{0}};
  Int_t L3ANumOfEffEntries[NUM_SSD][NPoints] = {{0}};

  Int_t L1SNumOfEntriesWithMultiCut[NUM_SSD][NPoints] = {{0}};
  Int_t L2FNumOfEntriesWithMultiCut[NUM_SSD][NPoints] = {{0}};
  Int_t L2BNumOfEntriesWithMultiCut[NUM_SSD][NPoints] = {{0}};
  Int_t L3ANumOfEntriesWithMultiCut[NUM_SSD][NPoints] = {{0}};

  Double_t L1SMultiRatio[NUM_SSD][NPoints] = {{0}};
  Double_t L2FMultiRatio[NUM_SSD][NPoints] = {{0}};
  Double_t L2BMultiRatio[NUM_SSD][NPoints] = {{0}};
  Double_t L3AMultiRatio[NUM_SSD][NPoints] = {{0}};

  Int_t SSD_L1S_E[NUM_SSD][NUM_STRIP];
  Int_t SSD_L2F_E[NUM_SSD][NUM_STRIP];
  Int_t SSD_L2B_E[NUM_SSD][NUM_STRIP];
  Int_t SSD_L3A_E[NUM_SSD][NUM_CSI];

  Double_t* L1SEChCut[NPoints];
  Double_t* L2FEChCut[NPoints];
  Double_t* L2BEChCut[NPoints];
  Double_t* L3AEChCut[NPoints];

  CSHINESSDCalibratedData     calidata;
  CSHINEHitPatternRecognition hitpattern;

  // 计算每一层的 EChCut
  for (Int_t i=0; i<NPoints; i++)
  {
    L1SEChCut[i] = calidata.GetSiEChCut (L1STag.c_str(), SigmaIndex[i]);
    L2FEChCut[i] = calidata.GetSiEChCut (L2FTag.c_str(), SigmaIndex[i]);
    L2BEChCut[i] = calidata.GetSiEChCut (L2BTag.c_str(), SigmaIndex[i]);
    L3AEChCut[i] = calidata.GetCsIEChCut(L3ATag.c_str(), SigmaIndex[i]);
  }

  TChain* mychain = new TChain("KrPb");
  for (Int_t i=FirstRun; i<(LastRun+1); i++) {
    mychain->Add(Form("%sMapFission2019_Kr_Pb%04d.root",pathRootInputFolder.c_str(),i));
  }
  mychain->SetBranchStatus("*",false);

  // set branchadress
  ReadFileModule readfile;
  for (Int_t i=0; i<NUM_SSD; i++)
  {
    readfile.AddChain(mychain, L1STag.c_str(), SSD_L1S_E[i], NUM_STRIP, i);
    readfile.AddChain(mychain, L2FTag.c_str(), SSD_L2F_E[i], NUM_STRIP, i);
    readfile.AddChain(mychain, L2BTag.c_str(), SSD_L2B_E[i], NUM_STRIP, i);
    readfile.AddChain(mychain, L3ATag.c_str(), SSD_L3A_E[i], NUM_CSI,   i);
  }

  //____________________________________________________________________________
  //                       逐一读取事件进行分析
  unsigned long nentries = mychain->GetEntries();
  cout<< "Found " << nentries <<" entries"<<endl;
  for (unsigned long jentry=0; jentry<nentries; jentry++)
  {
    mychain->GetEntry(jentry);

   for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++)
   {
     for (Int_t np=0; np<NPoints; np++)
     {
       L1SMulti[ssdindex][np] = hitpattern.LayerMultiplicity(ssdindex, L1STag.c_str(), SSD_L1S_E[ssdindex], L1SEChCut[np]);
       L2FMulti[ssdindex][np] = hitpattern.LayerMultiplicity(ssdindex, L2FTag.c_str(), SSD_L2F_E[ssdindex], L2FEChCut[np]);
       L2BMulti[ssdindex][np] = hitpattern.LayerMultiplicity(ssdindex, L2BTag.c_str(), SSD_L2B_E[ssdindex], L2BEChCut[np]);
       L3AMulti[ssdindex][np] = hitpattern.LayerMultiplicity(ssdindex, L3ATag.c_str(), SSD_L3A_E[ssdindex], L3AEChCut[np]);

       AddNumOfCount(L1SMulti[ssdindex][np], L1SNumOfEffEntries[ssdindex][np], L1SNumOfEntriesWithMultiCut[ssdindex][np]);
       AddNumOfCount(L2FMulti[ssdindex][np], L2FNumOfEffEntries[ssdindex][np], L2FNumOfEntriesWithMultiCut[ssdindex][np]);
       AddNumOfCount(L2BMulti[ssdindex][np], L2BNumOfEffEntries[ssdindex][np], L2BNumOfEntriesWithMultiCut[ssdindex][np]);
       AddNumOfCount(L3AMulti[ssdindex][np], L3ANumOfEffEntries[ssdindex][np], L3ANumOfEntriesWithMultiCut[ssdindex][np]);
     }
   }
   time.PrintPercentageAndRemainingTime(jentry, nentries);
  }

  //____________________________________________________________________________
  // 计算 MultiRatio
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++)
  {
    for (Int_t np=0; np<NPoints; np++)
    {
      L1SMultiRatio[ssdindex][np] = EventRatioWithMultiCut(ssdindex,L1STag.c_str(),L1SNumOfEffEntries[ssdindex][np],
                                      L1SNumOfEntriesWithMultiCut[ssdindex][np]);
      L2FMultiRatio[ssdindex][np] = EventRatioWithMultiCut(ssdindex,L2FTag.c_str(),L2FNumOfEffEntries[ssdindex][np],
                                      L2FNumOfEntriesWithMultiCut[ssdindex][np]);
      L2BMultiRatio[ssdindex][np] = EventRatioWithMultiCut(ssdindex,L2BTag.c_str(),L2BNumOfEffEntries[ssdindex][np],
                                      L2BNumOfEntriesWithMultiCut[ssdindex][np]);
      L3AMultiRatio[ssdindex][np] = EventRatioWithMultiCut(ssdindex,L3ATag.c_str(),L3ANumOfEffEntries[ssdindex][np],
                                      L3ANumOfEntriesWithMultiCut[ssdindex][np]);

    //  cout<<L1SMultiRatio[ssdindex][np]<<endl;
    }
  }

  //____________________________________________________________________________
  // TGraph 画图
  TGraph* gr_L1SMulti[NUM_SSD];
  TGraph* gr_L2FMulti[NUM_SSD];
  TGraph* gr_L2BMulti[NUM_SSD];
  TGraph* gr_L3AMulti[NUM_SSD];
  for (Int_t ssdindex=0; ssdindex<NUM_SSD; ssdindex++)
  {
    //gr_L1SMulti[ssdindex] = new TGraph(NPoints, );
  }

  time.GetEndTime();
  time.GetRunTime();
}


//______________________________________________________________________________
void AddNumOfCount(Int_t multi, Int_t& effentries, Int_t& entrieswithmulticut)
{
  if (multi >= 1) {
    effentries++;
    if (multi <= MultiCut) {
      entrieswithmulticut++;
    }
  }
}


//______________________________________________________________________________
Double_t EventRatioWithMultiCut(Int_t ssdindex, const char* layertag, Int_t effentries,
  Int_t entriswithmulticut)
{
  Double_t ratio;
  if (effentries == 0) {
    cout<<Form("Error: Multiplicity of SSD%d_%s is 0!",ssdindex+1, layertag)<<endl;
  } else {
    ratio = (Double_t) (entriswithmulticut/effentries);
  }
  return ratio;
}
