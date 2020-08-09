#include "../include/CSHINEBuildEvent.h"
#include "../include/CSHINEBuildEvent.h"
#include "../include/CSHINEEvent.h"
#include "../include/ReadFileModule.h"
#include "../include/TimeAndPercentage.h"
#include "../include/CSHINESSDCalibratedData.h"


Int_t FirstRun = 200;
Int_t LastRun  = 203;
Int_t NumSigma = 10;

//______________________________________________________________________________
void test5()
{
  TimeAndPercentage time;
  time.GetBeginTime();

  std::string L1STag("L1S");
  std::string L2FTag("L2F");
  std::string L2BTag("L2B");
  std::string L3ATag("L3A");
  std::string pathRootInputFolder("/home/sea/Fission2019_Data/MapRoot/");

  Int_t SSD_L1S_E[NUM_SSD][NUM_STRIP];
  Int_t SSD_L2F_E[NUM_SSD][NUM_STRIP];
  Int_t SSD_L2B_E[NUM_SSD][NUM_STRIP];
  Int_t SSD_L3A_E[NUM_SSD][NUM_CSI];

  TFile* file   = new TFile(Form("EventTree_Run%04d-Run%04d.root",FirstRun,LastRun),"RECREATE");
  TTree* mytree = new TTree("CSHINEEvent", "CSHINEEvent Tree");

  CSHINEBuildEvent buildevent;

  CSHINESSDEvent    event1;
  CSHINESSDEvent    event2;
  CSHINESSDEvent    event3;
  CSHINESSDEvent    event4;
  CSHINEGlobalEvent* event_g = new CSHINEGlobalEvent();

  mytree->Branch("SSD1.",   "CSHINESSDEvent",    &event1,  32000, 2);
  mytree->Branch("SSD2.",   "CSHINESSDEvent",    &event2,  32000, 2);
  mytree->Branch("SSD3.",   "CSHINESSDEvent",    &event3,  32000, 2);
  mytree->Branch("SSD4.",   "CSHINESSDEvent",    &event4,  32000, 2);
  mytree->Branch("Global.", "CSHINEGlobalEvent", &event_g, 32000, 2);

  TChain* mychain = new TChain("KrPb");
  for (Int_t i=FirstRun; i<(LastRun+1); i++) {
    mychain->Add(Form("%sMapFission2019_Kr_Pb%04d.root",pathRootInputFolder.c_str(),i));
  }
  mychain->SetBranchStatus("*",false);

  ReadFileModule readfile;
  for (Int_t i=0; i<NUM_SSD; i++)
  {
    readfile.AddChain(mychain,L1STag.c_str(),SSD_L1S_E[i],NUM_STRIP,i);
    readfile.AddChain(mychain,L2FTag.c_str(),SSD_L2F_E[i],NUM_STRIP,i);
    readfile.AddChain(mychain,L2BTag.c_str(),SSD_L2B_E[i],NUM_STRIP,i);
    readfile.AddChain(mychain,L3ATag.c_str(),SSD_L3A_E[i],NUM_CSI,  i);
  }

  //____________________________________________________________________________
  // 逐一读取事件进行分析
  unsigned long nentries = mychain->GetEntries();
  cout<< "Found " << nentries <<" entries"<<endl;
  for (unsigned long j=0; j<nentries; j++)
  {
    mychain->GetEntry(j);

    buildevent.BuildSSDEvent(event1,0,SSD_L1S_E[0],SSD_L2F_E[0],SSD_L2B_E[0],SSD_L3A_E[0]);
    buildevent.BuildSSDEvent(event2,1,SSD_L1S_E[1],SSD_L2F_E[1],SSD_L2B_E[1],SSD_L3A_E[1]);
    buildevent.BuildSSDEvent(event3,2,SSD_L1S_E[2],SSD_L2F_E[2],SSD_L2B_E[2],SSD_L3A_E[2]);
    buildevent.BuildSSDEvent(event4,3,SSD_L1S_E[3],SSD_L2F_E[3],SSD_L2B_E[3],SSD_L3A_E[3]);

    mytree->Fill();

    time.PrintPercentageAndRemainingTime(j, nentries);
  }
  mytree->Write();
  file->Close();

  time.GetEndTime();
  time.GetRunTime();
}
