#include "../include/CSHINEEvent.h"


//******************************************************************************
// 定义 CSHINELayerEvent
CSHINELayerEvent::CSHINELayerEvent() :
fL1SMulti(0),
fL2FMulti(0),
fL2BMulti(0),
fCsIMulti(0)
{}

CSHINELayerEvent::~CSHINELayerEvent()
{}
//******************************************************************************


//******************************************************************************
CSHINETrackEvent::CSHINETrackEvent()
{}

CSHINETrackEvent::~CSHINETrackEvent()
{}
//******************************************************************************


//******************************************************************************
// 定义 CSHINEGlobalEvent
CSHINEGlobalEvent::CSHINEGlobalEvent() : fGlobalMulti(0)
{}

CSHINEGlobalEvent::~CSHINEGlobalEvent()
{}
//******************************************************************************


//******************************************************************************
// 创建、填充 Event
CSHINEBuildEvent::CSHINEBuildEvent()
{
  fSiEChcutl1s   = fCSHINESSDCalibratedData.GetSiEChCut ("L1S");
  fSiEChcutl2f    = fCSHINESSDCalibratedData.GetSiEChCut ("L2F");
  fSiEChcutl2b   = fCSHINESSDCalibratedData.GetSiEChCut ("L2B");
  fCsIEChcutl3a = fCSHINESSDCalibratedData.GetCsIEChCut("L3A");

  fSlopel1s   = fCSHINESSDCalibratedData.GetSiCaliSlope("L1S");
  fSlopel2f    = fCSHINESSDCalibratedData.GetSiCaliSlope("L2F");
  fSlopel2b   = fCSHINESSDCalibratedData.GetSiCaliSlope("L2B");

  fInterceptl1s = fCSHINESSDCalibratedData.GetSiCaliIntercept("L1S");
  fInterceptl2f  = fCSHINESSDCalibratedData.GetSiCaliIntercept("L2F");
  fInterceptl2b = fCSHINESSDCalibratedData.GetSiCaliIntercept("L2B");
}

//______________________________________________________________________________
CSHINEBuildEvent::~CSHINEBuildEvent()
{}
//******************************************************************************


//******************************************************************************
void CSHINEBuildEvent::LayerEvent(CSHINELayerEvent& layerevent, Int_t ssdindex,
  Int_t* echl1s, Int_t* echl2f, Int_t* echl2b, Int_t* echcsi)
{
  SSDL1SMulti=0, SSDL2FMulti=0, SSDL2BMulti=0, SSDCsIMulti=0;

  for (Int_t chindex=0; chindex<NUM_STRIP; chindex++) {
    // for L1S
    if (echl1s[chindex]>fSiEChcutl1s[ssdindex*NUM_STRIP+chindex]) {
      SSDL1SMulti++;
      layerevent.fL1SSSDNum.push_back(ssdindex);
      layerevent.fL1SNumStrip.push_back(chindex);
      layerevent.fL1SEMeV.push_back(fSlopel1s[ssdindex*NUM_STRIP+chindex]*echl1s[chindex]+fInterceptl1s[ssdindex*NUM_STRIP+chindex]);
    }
    // for L2F
    if (echl2f[chindex]>fSiEChcutl2f[ssdindex*NUM_STRIP+chindex]) {
      SSDL2FMulti++;
      layerevent.fL2FSSDNum.push_back(ssdindex);
      layerevent.fL2FNumStrip.push_back(chindex);
      layerevent.fL2FEMeV.push_back(fSlopel2f[ssdindex*NUM_STRIP+chindex]*echl2f[chindex]+fInterceptl2f[ssdindex*NUM_STRIP+chindex]);
    }
    // for L2B
    if (echl2b[chindex]>fSiEChcutl2b[ssdindex*NUM_STRIP+chindex]) {
      SSDL2BMulti++;
      layerevent.fL2BSSDNum.push_back(ssdindex);
      layerevent.fL2BNumStrip.push_back(chindex);
      layerevent.fL2BEMeV.push_back(fSlopel2b[ssdindex*NUM_STRIP+chindex]*echl2b[chindex]+fInterceptl2b[ssdindex*NUM_STRIP+chindex]);
    }
  }
  // for L3A
  for (Int_t chindex=0; chindex<NUM_CSI; chindex++) {
    if (echcsi[chindex]>fCsIEChcutl3a[ssdindex*NUM_CSI+chindex]) {
      SSDCsIMulti++;
      layerevent.fCsISSDNum.push_back(ssdindex);
      layerevent.fCsINum.push_back(chindex);
      layerevent.fCsIECh.push_back(echcsi[chindex]);
    }
  }
  layerevent.fSSDL1SMulti.push_back(SSDL1SMulti);
  layerevent.fSSDL2FMulti.push_back(SSDL2FMulti);
  layerevent.fSSDL2BMulti.push_back(SSDL2BMulti);
  layerevent.fSSDCsIMulti.push_back(SSDCsIMulti);
}

//______________________________________________________________________________
void CSHINEBuildEvent::BuildLayerEvent(CSHINELayerEvent& layerevent,
    Int_t* ssd1echl1s, Int_t* ssd1echl2f, Int_t* ssd1echl2b, Int_t* ssd1echcsi,
    Int_t* ssd2echl1s, Int_t* ssd2echl2f, Int_t* ssd2echl2b, Int_t* ssd2echcsi,
    Int_t* ssd3echl1s, Int_t* ssd3echl2f, Int_t* ssd3echl2b, Int_t* ssd3echcsi,
    Int_t* ssd4echl1s, Int_t* ssd4echl2f, Int_t* ssd4echl2b, Int_t* ssd4echcsi)
{
  //_________________________________________________
  // 1.获取 multipicity, 并将点火的 channel index 存进 vector
  layerevent.fL1SMulti  = 0;
  layerevent.fL2FMulti  = 0;
  layerevent.fL2BMulti  = 0;
  layerevent.fCsIMulti  = 0;

  LayerEvent(layerevent, 0, ssd1echl1s, ssd1echl2f, ssd1echl2b, ssd1echcsi);
  LayerEvent(layerevent, 1, ssd2echl1s, ssd2echl2f, ssd2echl2b, ssd2echcsi);
  LayerEvent(layerevent, 2, ssd3echl1s, ssd3echl2f, ssd3echl2b, ssd3echcsi);
  LayerEvent(layerevent, 3, ssd4echl1s, ssd4echl2f, ssd4echl2b, ssd4echcsi);
  for (Int_t i=0; i<NUM_SSD; i++) {
    layerevent.fL1SMulti += layerevent.fSSDL1SMulti[i];
    layerevent.fL2FMulti += layerevent.fSSDL2FMulti[i];
    layerevent.fL2BMulti += layerevent.fSSDL2BMulti[i];
    layerevent.fCsIMulti += layerevent.fSSDCsIMulti[i];
  }

}
//******************************************************************************


//******************************************************************************
// 创建 LayerEvent 的树结构
void CSHINEBuildEvent::BuildLayerEventTree(Int_t firstrun, Int_t lastrun)
{
  TimeAndPercentage time;
  time.GetBeginTime();

  std::string L1STag("L1S");
  std::string L2FTag("L2F");
  std::string L2BTag("L2B");
  std::string L3ATag("L3A");

  std::string pathRootInputFolder(Form("%sMapRoot/", PATHROOTFILESFOLDER));
  std::string pathEventTreeOutput(Form("%sCSHINEEvent/", PATHROOTFILESFOLDER));

  Int_t SSD_L1S_E[NUM_SSD][NUM_STRIP];
  Int_t SSD_L2F_E[NUM_SSD][NUM_STRIP];
  Int_t SSD_L2B_E[NUM_SSD][NUM_STRIP];
  Int_t SSD_L3A_E[NUM_SSD][NUM_CSI];

  cout<<Form("Build Event for Run%04d-Run%04d", firstrun, lastrun)<<endl;
  TFile* file   = new TFile(Form("%sEventTree_Run%04d-Run%04d.root",pathEventTreeOutput.c_str(),
                                  firstrun, lastrun), "RECREATE");
  TTree* mytree = new TTree("LayerEvent", "CSHINE SSDLayerEvent");

  CSHINEBuildEvent    buildevent;
  CSHINELayerEvent    layerevent;

  mytree->Branch("LayerEvent.", "CSHINELayerEvent", &layerevent,  32000, 2);

  TChain* mychain = new TChain("KrPb");
  for (Int_t i=firstrun; i<(lastrun+1); i++) {
    mychain->Add(Form("%sMapFission2019_Kr_Pb%04d.root",pathRootInputFolder.c_str(),i));
    //mychain->Add(Form("%sMapSSD_L2_AlphaCali%04d.root",pathRootInputFolder.c_str(),i));
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
    time.PrintPercentageAndRemainingTime(jentry, nentries);

    buildevent.BuildLayerEvent(layerevent,
                 SSD_L1S_E[0],SSD_L2F_E[0],SSD_L2B_E[0],SSD_L3A_E[0],
                 SSD_L1S_E[1],SSD_L2F_E[1],SSD_L2B_E[1],SSD_L3A_E[1],
                 SSD_L1S_E[2],SSD_L2F_E[2],SSD_L2B_E[2],SSD_L3A_E[2],
                 SSD_L1S_E[3],SSD_L2F_E[3],SSD_L2B_E[3],SSD_L3A_E[3]);
    mytree->Fill();
    layerevent.Swap();
  }
  mytree->Write();
  file->Close();

  time.GetEndTime();
  time.GetRunTime();
}
//******************************************************************************


//******************************************************************************
void CSHINEBuildEvent::BuildSSDEventTree(Int_t fisrtrun, Int_t lastlun)
{}

//******************************************************************************


//******************************************************************************
void CSHINEBuildEvent::BuildGlobalEvent(CSHINEGlobalEvent&, Int_t*, Int_t*, Int_t*, Int_t*)
{}


//******************************************************************************
void CSHINEBuildEvent::BuildGlobalEventTree(Int_t firstrun, Int_t lastrun)
{}
