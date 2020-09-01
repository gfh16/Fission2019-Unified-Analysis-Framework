////////////////////////////////////////////////////////////////////////////////
//
//  创建 CSHINE 的事件. 事件的创建分为以下几步:
//   1. 定义数据存储结构. [CSHINEEvent.h]
//      CSHINESSDEvent    -- 定义独立探测器事件
//      CSHINEGlobalEvent -- 定义全局事件
//
//   2. 创建数据填充函数. [CSHINEBuildEvent.h]
//      对前面 CSHINESSDEvent，CSHINEGlobalEvent 的事件创建、填充
//
//  3. 利用 TChain 添加数据, 逐个事件读取、分析
//
//  ** 说明:
//  (1)关于硅条 Cut 的选取: 可调节 shared.C 中的 NUM_SIGMA
//     目前 NUM_SIGAM = 10, L1S=2*10 sigma, L2F = L2B = 10*sigma
//  (2)CsI 使用同一的 Cut = 150.0  [可用 GenerateEChCut.C 修改]
//
//  gfh, 2020-07-30
////////////////////////////////////////////////////////////////////////////////

#include "../../include/CSHINEEvent.h"
#include "../../include/CSHINEBuildEvent.h"
#include "../../include/ReadFileModule.h"
#include "../../include/TimeAndPercentage.h"
#include "../../include/CSHINESSDCalibratedData.h"


Int_t FirstRun = 200;
Int_t LastRun  = 207;

//______________________________________________________________________________
void EventTree_BuildCSHINEEventTree()
{
  TimeAndPercentage time;
  time.GetBeginTime();

  std::string L1STag("L1S");
  std::string L2FTag("L2F");
  std::string L2BTag("L2B");
  std::string L3ATag("L3A");

  std::string pathRootInputFolder("/home/sea/Fission2019_Data/MapRoot/");
  std::string pathEventTreeOutput("/home/sea/Fission2019_Data/CSHINEEvent/");

  Int_t SSD_L1S_E[NUM_SSD][NUM_STRIP];
  Int_t SSD_L2F_E[NUM_SSD][NUM_STRIP];
  Int_t SSD_L2B_E[NUM_SSD][NUM_STRIP];
  Int_t SSD_L3A_E[NUM_SSD][NUM_CSI];

  cout<<Form("Build Event for Run%04d-Run%04d", FirstRun, LastRun)<<endl;
  TFile* file   = new TFile(Form("%sEventTree_Run%04d-Run%04d.root",pathEventTreeOutput.c_str(),
                                  FirstRun,LastRun),"RECREATE");
  TTree* mytree = new TTree("CSHINEEvent", "CSHINEEvent Tree");

  CSHINEBuildEvent buildevent;

  CSHINESSDEvent    event1;
  CSHINESSDEvent    event2;
  CSHINESSDEvent    event3;
  CSHINESSDEvent    event4;
  CSHINEGlobalEvent event_g;

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
  for (unsigned long jentry=0; jentry<nentries; jentry++)
  {
    mychain->GetEntry(jentry);

    buildevent.BuildSSDEvent(event1,0,SSD_L1S_E[0],SSD_L2F_E[0],SSD_L2B_E[0],SSD_L3A_E[0]);
    buildevent.BuildSSDEvent(event2,1,SSD_L1S_E[1],SSD_L2F_E[1],SSD_L2B_E[1],SSD_L3A_E[1]);
    buildevent.BuildSSDEvent(event3,2,SSD_L1S_E[2],SSD_L2F_E[2],SSD_L2B_E[2],SSD_L3A_E[2]);
    buildevent.BuildSSDEvent(event4,3,SSD_L1S_E[3],SSD_L2F_E[3],SSD_L2B_E[3],SSD_L3A_E[3]);

    mytree->Fill();

    time.PrintPercentageAndRemainingTime(jentry, nentries);
  }
  mytree->Write();
  file->Close();

  time.GetEndTime();
  time.GetRunTime();
}
